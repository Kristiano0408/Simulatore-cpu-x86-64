from PySide6.QtWidgets import QDockWidget, QScrollArea, QVBoxLayout,QHBoxLayout, QLabel, QTableWidget, QTableWidgetItem, QHeaderView, QWidget, QFrame
from PySide6.QtGui import QFont, Qt,QColor
import PySide6.QtCore as QtCore

import simulator

DockFeature = QDockWidget.DockWidgetFeature


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



class CacheDock(QDockWidget):
    def __init__(self, parent=None):
        super().__init__("", parent)

        self.parent = parent
        self.L1_cache : simulator.CacheLevel = None
        self.L2_cache : simulator.CacheLevel = None
        self.L3_cache : simulator.CacheLevel = None
        self.memory_scheduler : simulator.MemoryScheduler = None

        self.setFeatures(QDockWidget.DockWidgetFloatable | QDockWidget.DockWidgetMovable)
        self.setMinimumWidth(400)
        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.setWidget(self.scroll_area)
        self.cache_dock_mainframe = QWidget()
        self.scroll_area.setWidget(self.cache_dock_mainframe)
        self.main_layout = QVBoxLayout(self.cache_dock_mainframe)
        self.main_layout.setAlignment(Qt.AlignTop)

        self.title_layout = QHBoxLayout()
        self.main_layout.addLayout(self.title_layout)
        title = QLabel(self.cache_dock_mainframe, text="Cache Viewer", font=QFont("Arial", 16, QFont.Bold))
        self.title_layout.addWidget(title)

        self.secondary_layout = QHBoxLayout()
        self.main_layout.addLayout(self.secondary_layout)

        

        self.queue_layout = QVBoxLayout()
        self.secondary_layout.addLayout(self.queue_layout)
    
        self.queue_tables = {}
        self.queue_names = ["L1", "L2", "L3", "Memory"]

        for i, name in enumerate(self.queue_names):
            col_layout = QVBoxLayout()
            label = QLabel(text=f"{name} Request Queue", font=QFont("Arial", 12, QFont.Bold))
            col_layout.addWidget(label)

            table = QTableWidget()
            table.setColumnCount(3)
            table.horizontalHeader().setVisible(False)
            table.verticalHeader().setVisible(False)
            table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
            table.setEditTriggers(QTableWidget.NoEditTriggers)
            table.setSelectionMode(QTableWidget.NoSelection)
            table.setFocusPolicy(QtCore.Qt.NoFocus)
            table.setMinimumHeight(120)
            table.setMaximumWidth(300)

            col_layout.addWidget(table)
            self.queue_tables[name] = table
            self.queue_layout.addLayout(col_layout)
        

        processing_layout = QVBoxLayout()
        self.secondary_layout.addLayout(processing_layout)

        self.processing_labels = {}
        self.processing_names = ["L1", "L2", "L3", "Memory"]

        for i, name in enumerate(self.processing_names):
            label = QLabel(text=f"{name} Processing Request: (none)", font=QFont("Arial", 12, QFont.Bold))
            processing_layout.addWidget(label)
            self.processing_labels[name] = label

        self.update_cache_view()

    
    
    def update_cache_view(self):
        # Aggiorna la vista della cache
        self.L1_queue = self.parent.cache.getL1RequestQueue()
        self.L2_queue = self.parent.cache.getL2RequestQueue()
        self.L3_queue = self.parent.cache.getL3RequestQueue()
        self.memory_queue = self.parent.cache.getMemoryRequestQueue()

        self.update_queue_table("L1", self.L1_queue)
        self.update_queue_table("L2", self.L2_queue)
        self.update_queue_table("L3", self.L3_queue)
        self.update_queue_table("Memory", self.memory_queue)


    def update_queue_table(self, name, queue):
        table = self.queue_tables[name]
        table.setRowCount(len(queue))
        for row, request in enumerate(queue):
            type_item = QTableWidgetItem(request.request.type.name)
            state_item = QTableWidgetItem(request.requestState.name)
            latency_item = QTableWidgetItem(str(request.remainingLatency))

            state_item.setTextAlignment(Qt.AlignCenter)
            type_item.setTextAlignment(Qt.AlignCenter)
            latency_item.setTextAlignment(Qt.AlignCenter)

            table.setItem(row, 0, latency_item)
            table.setItem(row, 1, type_item)
            table.setItem(row, 2, state_item)


class RegisterDock(QDockWidget):
    def __init__(self, parent=None):
        super().__init__("", parent)
        self.parent = parent
        self.setFeatures(QDockWidget.DockWidgetFloatable | QDockWidget.DockWidgetMovable)
        self.setMinimumWidth(150)


        self.register_dock_mainframe = QWidget()
        self.setWidget(self.register_dock_mainframe)
        

        self.main_layout = QVBoxLayout(self.register_dock_mainframe)
        self.main_layout.setAlignment(Qt.AlignTop)

        self.title = QLabel(self.register_dock_mainframe, text="Register Viewer", font=QFont("Arial", 16, QFont.Bold))
        self.layout_title = QHBoxLayout()
        self.layout_title.addWidget(self.title)
        self.main_layout.addLayout(self.layout_title)

        self.secondary_layout = QHBoxLayout()
        self.main_layout.addLayout(self.secondary_layout)

        self.first_register_column = QVBoxLayout()
        self.secondary_layout.addLayout(self.first_register_column)

        self.second_register_column = QVBoxLayout()
        self.secondary_layout.addLayout(self.second_register_column)

        self.third_register_column = QVBoxLayout()
        self.secondary_layout.addLayout(self.third_register_column)

        self.fourth_register_column = QVBoxLayout()
        self.secondary_layout.addLayout(self.fourth_register_column)

        self.labels = {}
        for i, name in enumerate(REGISTER_NAMES):
            label = QLabel(self.register_dock_mainframe, text=f"{name}: 0")
            if i < 4:
                self.first_register_column.addWidget(label)
            elif i < 8:
                self.second_register_column.addWidget(label)
            elif i < 12:
                self.third_register_column.addWidget(label)
            else:
                self.fourth_register_column.addWidget(label)
            self.labels[name] = label

        self.update_register_labels()  


    def update_register_labels(self):
        for name in REGISTER_NAMES:
            enum_val = getattr(simulator, name)
            value = self.parent.registers.getReg(enum_val).raw()
            value_hex = f"0x{value:016X}"
            self.labels[name].setText(str(f"{name}: {value_hex}"))

class MemoryDock(QDockWidget):
    def __init__(self, parent=None):
        super().__init__("", parent)
        self.parent = parent
        self.setFeatures(QDockWidget.DockWidgetFloatable | QDockWidget.DockWidgetMovable)
        self.setMinimumWidth(400)

        
        self.memory_dock_mainframe = QWidget()
        self.setWidget(self.memory_dock_mainframe)
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
        
        mem_data = self.parent.memory.getData()
        rip = self.parent.registers.getReg(simulator.RIP).raw()

        #start = max(0, rip - 5)
        #end = min(len(mem_data), rip + 50)
        start = 0
        end = 1100

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

class PipelineDock(QDockWidget):
    def __init__(self, parent=None):
        super().__init__("", parent)
        self.parent = parent
        self.setFeatures(QDockWidget.DockWidgetFloatable | QDockWidget.DockWidgetMovable)
        self.setMinimumWidth(400)
        self.pipeline_dock_mainframe = QWidget()

        self.scroll_area = QScrollArea()
        self.scroll_area.setWidgetResizable(True)
        self.scroll_area.setWidget(self.pipeline_dock_mainframe)
        self.setWidget(self.scroll_area)

        self.pipeline_dock_main_layout = QHBoxLayout(self.pipeline_dock_mainframe)
        self.pipeline_dock_mainframe.setLayout(self.pipeline_dock_main_layout)
        

        self.pipeline_dock_stages_layout = QVBoxLayout()
        self.pipeline_dock_buffers_layout = QVBoxLayout()
        self.pipeline_dock_main_layout.addLayout(self.pipeline_dock_stages_layout)
        self.pipeline_dock_main_layout.addLayout(self.pipeline_dock_buffers_layout)


        self.stage_labels = {}
        self.buffer_labels = {}

        # --- STAGES ---

        stages_title = QLabel(self.pipeline_dock_mainframe, text="Pipeline Stages", font=QFont("Arial", 14, QFont.Bold))
        self.pipeline_dock_stages_layout.addWidget(stages_title)

        for stage in PIPELINE_STAGES:
            frame = QFrame(self.pipeline_dock_mainframe)
            self.pipeline_dock_stages_layout.addWidget(frame)

            label_title = QLabel(frame, text=stage, font=QFont("Arial", 10, QFont.Bold))
            self.pipeline_dock_stages_layout.addWidget(label_title)

            label_text = QLabel(frame, text="(empty)", font=QFont("Arial", 10))
            self.pipeline_dock_stages_layout.addWidget(label_text)

            self.stage_labels[stage] = label_text

        buffers_title = QLabel(self.pipeline_dock_mainframe, text="Pipeline Buffers", font=QFont("Arial", 14, QFont.Bold))
        self.pipeline_dock_buffers_layout.addWidget(buffers_title)

        for buf in PIPELINE_BUFFERS:
            frame = QFrame(self.pipeline_dock_mainframe)
            self.pipeline_dock_buffers_layout.addWidget(frame)

            label_title = QLabel(frame, text=buf, font=QFont("Arial", 10, QFont.Bold))
            self.pipeline_dock_buffers_layout.addWidget(label_title)

            label_text = QLabel(frame, text="(empty)", font=QFont("Arial", 10))
            self.pipeline_dock_buffers_layout.addWidget(label_text)

            self.buffer_labels[buf] = label_text

        self.update_pipeline_view()
    
    
    def update_pipeline_view(self):
        try:
            
            stages = [self.parent.pipeline.getFetchStage(),
                      self.parent.pipeline.getDecodeStage(),
                      self.parent.pipeline.getOperandFetchStage(),
                      self.parent.pipeline.getExecuteStage(),
                      self.parent.pipeline.getMemoryStage(),
                      self.parent.pipeline.getWriteBackStage()]
            buffers = [self.parent.pipeline.getFetchDecodeBuffer(),
                       self.parent.pipeline.getDecodeOperandFetchBuffer(),
                       self.parent.pipeline.getOperandFetchExecuteBuffer(),
                       self.parent.pipeline.getExecuteMemoryBuffer(),
                       self.parent.pipeline.getMemoryWriteBackBuffer()]

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
                    instr_id = instr.getCore().InstructionId
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
                    instr_id = instr.getCore().InstructionId
            elif hasattr(buf, "instructionInfo"):
                instr_info = buf.instructionInfo
                if instr_info is not None:
                    instr_id = instr_info.instructionId if instr_info.instructionId != "0" else "(empty)"

            self.buffer_labels[name].setText(str(instr_id))

