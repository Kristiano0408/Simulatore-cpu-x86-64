import simulator
from PySide6.QtWidgets import QApplication, QHeaderView,QMainWindow,QFrame,QWidget,QDockWidget, QLabel, QVBoxLayout, QHBoxLayout, QTextEdit, QTableWidget, QTableWidgetItem, QScrollArea
from PySide6.QtGui import QFont, QShortcut, QKeySequence, Qt, QColor
import PySide6.QtCore as QtCore
import sys

REGISTER_NAMES = [
    "RAX","RBX","RCX","RDX",
    "RSI","RDI","RSP","RBP",
    "R8","R9","R10","R11",
    "R12","R13","R14","R15",
    "RIP"
]

PIPELINE_STAGES = [
    "FETCH","DECODE","OPERAND FETCH",
    "EXECUTE","MEMORY","WRITEBACK"
]

PIPELINE_BUFFERS = [
    "F/D","D/OF","OF/EX",
    "EX/MEM","MEM/WB"
]

DockFeature = QDockWidget.DockWidgetFeature
DockArea = QtCore.Qt.DockWidgetArea


class SimulatorGUI(QMainWindow):

       
    
    def __init__(self):
        super().__init__()
        self.resize(1200, 800)
        

        self.setCentralWidget(None)
        self.register_dock = self.create_dock("")
        self.register_dock.setMinimumWidth(150)
        self.cache_dock = self.create_dock("")
        self.cache_dock.setMinimumWidth(400)
        self.pipeline_dock = self.create_dock("")
        self.pipeline_dock.setMinimumWidth(400)
        self.memory_dock = self.create_dock("")
        self.memory_dock.setMinimumWidth(400)

        self.addDockWidget(DockArea.TopDockWidgetArea, self.register_dock)
        self.addDockWidget(DockArea.LeftDockWidgetArea, self.memory_dock)
        self.addDockWidget(DockArea.RightDockWidgetArea, self.cache_dock)
        self.addDockWidget(DockArea.BottomDockWidgetArea, self.pipeline_dock)

        self.bus = simulator.Bus()
        self.cpu = self.bus.getCPU()
        self.memory = self.bus.getMemory()
        self.registers = self.cpu.getRegisters()
        self.pipeline = self.cpu.getPipeline()

        self.memory.setDataPartial([
            0x2C, 0x03,
            0x2D, 0x05,0x00,0x00,0x00,
            0x80, 0xEB, 0x01,
            0x81, 0xE9, 0xFF,0xFF,0xFF,0xFF,
            0x83, 0xEA, 0x02
        ], 0)

        self.fill_register_dock(self.register_dock)
        self.fill_memory_dock(self.memory_dock)
        self.fill_pipeline_dock(self.pipeline_dock)
        self.fill_cache_dock(self.cache_dock)

        shortcut = QShortcut(QKeySequence("Space"), self)
        shortcut.activated.connect(self.on_space_press)

        

        


    def create_dock(self, name : str) -> QDockWidget:
        dock = QDockWidget(name,self)
        dock.setFeatures(DockFeature.DockWidgetFloatable | DockFeature.DockWidgetMovable)
        #inner = QWidget()
        #inner.setMinimumSize(300,300)
        #dock.setWidget(inner)
        return dock

    #############################################################################################################

    def fill_register_dock(self, dock_parent: QDockWidget):
        
        self.register_dock_mainframe = QWidget()
        dock_parent.setWidget(self.register_dock_mainframe)
        

        main_layout = QVBoxLayout(self.register_dock_mainframe)
        self.register_dock_mainframe.setLayout(main_layout)
        main_layout.setAlignment(Qt.AlignTop)

        title = QLabel(self.register_dock_mainframe, text="Register Viewer", font=QFont("Arial", 16, QFont.Bold))
        layout_title = QHBoxLayout(self.register_dock_mainframe)
        layout_title.addWidget(title)
        main_layout.addLayout(layout_title)

        secondary_layout = QHBoxLayout(self.register_dock_mainframe)
        main_layout.addLayout(secondary_layout)

        first_register_column = QVBoxLayout(self.register_dock_mainframe)
        secondary_layout.addLayout(first_register_column)

        second_register_column = QVBoxLayout(self.register_dock_mainframe)
        secondary_layout.addLayout(second_register_column)

        third_register_column = QVBoxLayout(self.register_dock_mainframe)
        secondary_layout.addLayout(third_register_column)

        fourth_register_column = QVBoxLayout(self.register_dock_mainframe)
        secondary_layout.addLayout(fourth_register_column)

        self.labels = {}
        for i, name in enumerate(REGISTER_NAMES):
            label = QLabel(self.register_dock_mainframe, text=f"{name}: 0")
            if i < 4:
                first_register_column.addWidget(label)
            elif i < 8:
                second_register_column.addWidget(label)
            elif i < 12:
                third_register_column.addWidget(label)
            else:
                fourth_register_column.addWidget(label)
            self.labels[name] = label

        self.update_register_labels()

    def update_register_labels(self):
        for name in REGISTER_NAMES:
            enum_val = getattr(simulator, name)
            value = self.registers.getReg(enum_val).raw()
            value_hex = f"0x{value:016X}"
            self.labels[name].setText(str(f"{name}: {value_hex}"))

    ##########################################################################################################à
    
    def fill_memory_dock(self, dock_parent: QDockWidget):
        
        self.memory_dock_mainframe = QWidget()
        dock_parent.setWidget(self.memory_dock_mainframe)
        layout = QVBoxLayout(self.memory_dock_mainframe)
        self.memory_dock_mainframe.setLayout(layout)

        title = QLabel(self.memory_dock_mainframe, text="Memory Viewer", font=QFont("Arial", 16, QFont.Bold))
        layout.addWidget(title)
        self.memory_table = QTableWidget()
        self.memory_table.setColumnCount(2)
        self.memory_table.setHorizontalHeaderLabels(["Address", "Byte"])
        self.memory_table.verticalHeader().setVisible(False)
        self.memory_table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.memory_table.setEditTriggers(QTableWidget.NoEditTriggers)
        self.memory_table.setSelectionMode(QTableWidget.NoSelection)
        self.memory_table.setFocusPolicy(QtCore.Qt.NoFocus)

        layout.addWidget(self.memory_table)
    
        self.update_memory_view()

    def update_memory_view(self):
        
        mem_data = self.memory.getData()
        rip = self.registers.getReg(simulator.RIP).raw()

        start = max(0, rip - 5)
        end = min(len(mem_data), rip + 50)

        self.memory_table.setRowCount(end - start)

        
        for row, addr in enumerate(range(start, end)):

            addr_item = QTableWidgetItem(f"{addr:08X}")
            value_item = QTableWidgetItem(f"{mem_data[addr]:02X}")

            addr_item.setTextAlignment(Qt.AlignCenter)
            value_item.setTextAlignment(Qt.AlignCenter)

            # Evidenzia il RIP
            if addr == rip:
                addr_item.setBackground(QColor("blue"))
                value_item.setBackground(QColor("blue"))

            self.memory_table.setItem(row, 0, addr_item)
            self.memory_table.setItem(row, 1, value_item)



   
    ################################################################################################ààà

    def fill_pipeline_dock(self, dock_parent: QDockWidget):
        self.pipeline_dock_mainframe = QWidget()

        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        scroll_area.setWidget(self.pipeline_dock_mainframe)
        dock_parent.setWidget(scroll_area)

        #dock_parent.setWidget(self.pipeline_dock_mainframe)
        self.pipeline_dock_main_layout = QHBoxLayout(self.pipeline_dock_mainframe)
        self.pipeline_dock_mainframe.setLayout(self.pipeline_dock_main_layout)
        

        self.pipeline_dock_stages_layout = QVBoxLayout()
        self.pipeline_dock_buffers_layout = QVBoxLayout()
        self.pipeline_dock_main_layout.addLayout(self.pipeline_dock_stages_layout)
        self.pipeline_dock_main_layout.addLayout(self.pipeline_dock_buffers_layout)


        self.stage_labels = {}
        self.buffer_labels = {}

        # --- STAGES ---

        stages_title = QLabel(self.pipeline_dock_mainframe, text="Pipeline Stages", font=QFont("Arial", 16, QFont.Bold))
        self.pipeline_dock_stages_layout.addWidget(stages_title)

        for stage in PIPELINE_STAGES:
            frame = QFrame(self.pipeline_dock_mainframe)
            self.pipeline_dock_stages_layout.addWidget(frame)

            label_title = QLabel(frame, text=stage)
            self.pipeline_dock_stages_layout.addWidget(label_title)

            label_text = QLabel(frame, text="(empty)")
            self.pipeline_dock_stages_layout.addWidget(label_text)

            self.stage_labels[stage] = label_text

        buffers_title = QLabel(self.pipeline_dock_mainframe, text="Pipeline Buffers", font=QFont("Arial", 16, QFont.Bold))
        self.pipeline_dock_buffers_layout.addWidget(buffers_title)

        for buf in PIPELINE_BUFFERS:
            frame = QFrame(self.pipeline_dock_mainframe)
            self.pipeline_dock_buffers_layout.addWidget(frame)

            label_title = QLabel(frame, text=buf)
            self.pipeline_dock_buffers_layout.addWidget(label_title)

            label_text = QLabel(frame, text="(empty)")
            self.pipeline_dock_buffers_layout.addWidget(label_text)

            self.buffer_labels[buf] = label_text
        

    def update_pipeline_view(self):
        try:
            
            stages = [self.pipeline.getFetchStage(),
                      self.pipeline.getDecodeStage(),
                      self.pipeline.getOperandFetchStage(),
                      self.pipeline.getExecuteStage(),
                      self.pipeline.getMemoryStage(),
                      self.pipeline.getWriteBackStage()]
            buffers = [self.pipeline.getFetchDecodeBuffer(),
                       self.pipeline.getDecodeOperandFetchBuffer(),
                       self.pipeline.getOperandFetchExecuteBuffer(),
                       self.pipeline.getExecuteMemoryBuffer(),
                       self.pipeline.getMemoryWriteBackBuffer()]

        except:
            stages = [""] * len(PIPELINE_STAGES)
            buffers = [""] * len(PIPELINE_BUFFERS)

        # update stages
        for name, stage in zip(PIPELINE_STAGES, stages):
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
            self.stage_labels[name].setText(str(value))

        # update buffers
        for name,  buf in zip(PIPELINE_BUFFERS, buffers):
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

            self.buffer_labels[name].setText(str(instr_id))

        # next update

    #############################################################################################################


    def fill_cache_dock(self, dock_parent: QDockWidget):
        self.cache_dock_mainframe = QWidget()
        dock_parent.setWidget(self.cache_dock_mainframe)
        main_layout = QVBoxLayout(self.cache_dock_mainframe)
        self.cache_dock_mainframe.setLayout(main_layout)
        main_layout.setAlignment(Qt.AlignTop)

        title_layout = QHBoxLayout(self.cache_dock_mainframe)
        main_layout.addLayout(title_layout)
        title = QLabel(self.cache_dock_mainframe, text="Cache Viewer", font=QFont("Arial", 16, QFont.Bold))
        title_layout.addWidget(title)

        secondary_layout = QHBoxLayout(self.cache_dock_mainframe)
        main_layout.addLayout(secondary_layout)

        queue_layout = QVBoxLayout()
        secondary_layout.addLayout(queue_layout)

        processing_layout = QVBoxLayout()
        secondary_layout.addLayout(processing_layout)

        


        self.update_cache_view()
    
    def update_cache_view(self):
        # Aggiorna la vista della cache
        pass  # Implementa la logica per aggiornare la vista della cache qui
    ##############################################################################################################




    def on_space_press(self):
        print("STEP CPU")
        self.cpu.cpuStep()
        self.update_register_labels()
        self.update_pipeline_view()
        self.update_memory_view()  # Aggiorna la vista della memoria dopo lo step della CPU


