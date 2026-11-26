import simulator
from docks import CacheDock, RegisterDock, MemoryDock, PipelineDock
from menuBar import MenuBar
from PySide6.QtWidgets import QApplication,QMainWindow,QWidget, QFileDialog, QMessageBox
from PySide6.QtGui import QShortcut, QKeySequence, QAction
import PySide6.QtCore as QtCore
import sys


DockArea = QtCore.Qt.DockWidgetArea






class SimulatorGUI(QMainWindow):

       
    
    def __init__(self):
        super().__init__()
        self.resize(1200, 800)

        self.setMenuBar(MenuBar(self))
        self.menuBar().open_file_action.triggered.connect(self.open_program)
        self.menuBar().run_steps_action.triggered.connect(self.run_steps)
        self.menuBar().run_all_action.triggered.connect(self.run_all)



        self.bus = simulator.Bus()
        self.cpu = self.bus.getCPU()
        self.memory = self.bus.getMemory()
        self.registers = self.cpu.getRegisters()
        self.pipeline = self.cpu.getPipeline()
        self.cache = self.cpu.getCacheManager()
        self.cycle = 0
        self.finished = False

        self.setCentralWidget(QWidget())
        self.register_dock = RegisterDock(self)
        self.cache_dock = CacheDock(self)
        self.pipeline_dock = PipelineDock(self)
        self.memory_dock = MemoryDock(self)
        self.addDockWidget(DockArea.TopDockWidgetArea, self.register_dock)
        self.addDockWidget(DockArea.LeftDockWidgetArea, self.memory_dock)
        self.addDockWidget(DockArea.RightDockWidgetArea, self.cache_dock)
        self.addDockWidget(DockArea.BottomDockWidgetArea, self.pipeline_dock)

        

        """self.memory.setDataPartial([
            0x2C, 0x03,
            0x2D, 0x05,0x00,0x00,0x00,
            0x80, 0xEB, 0x01,
            0x81, 0xE9, 0xFF,0xFF,0xFF,0xFF,
            0x83, 0xEA, 0x02
        ], 0)"""
        """self.memory.setDataPartial([
            0xB1, 0x22,
            0x48, 0x89, 0x06,       # MOV [RSI], RAX
            0x41, 0xB8, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00,
        ], 0)"""

        self.registers.getReg(simulator.Register.RSI).setRaw(0x100)
        self.registers.getReg(simulator.Register.RAX).setRaw(0xCAFEBABE)
        self.register_dock.update_register_labels()

        

    def drain_logs(self):
        """Drain all EventLog queues to prevent overflow."""
        event_log = simulator.EventLog.getInstance()
        while event_log.getCacheDataLogSize() > 0:
            event_log.getCacheDataLogEntry()
        while event_log.getMemoryDataLogSize() > 0:
            event_log.getMemoryDataLogEntry()
        while event_log.getOperandDataLogSize() > 0:
            event_log.getOperandDataLogEntry()
        while event_log.getGeneralLogSize() > 0:
            event_log.getGeneralLogEntry()

    def program_loaded(self):
        memory_data = self.memory.getData()[0:1]
        if memory_data == [0x00]:
            print("Program Not loaded. Please load a program before executing steps.")
            print_dialog = QMessageBox(self)
            print_dialog.warning(self, "Program Not Loaded", "Program Not loaded. Please load a program before executing steps.")
            return False
        return True


    def on_space_press(self):

        if self.finished:
            print("Program already finished. No further steps can be executed.")
            return
        if(self.cycle >= 100):
            print("Program finished")
            self.cache.flushAllCaches()  # Flush all caches when the program finishes
            self.cache_dock.update_cache_view()  # Update the cache view after flushing
            self.memory_dock.update_memory_view()  # Update the memory view after flushing
            self.finished = True
            return
        self.cycle += 1
        print("STEP CPU")
        self.cpu.cpuStep()
        self.drain_logs()
        self.cache_dock.update_cache_view()
        self.register_dock.update_register_labels()
        self.pipeline_dock.update_pipeline_view()
        self.memory_dock.update_memory_view()  # Aggiorna la vista della memoria dopo lo step della CPU

    

###########################################################################################################################################

    def open_program(self):
        file_dialog = QFileDialog(self)
        file_dialog.setNameFilter("Program Files (*.bin, *.txt)")
        if file_dialog.exec():
            selected_files = file_dialog.selectedFiles()
            if selected_files:
                program_path = selected_files[0]
                type = "bin" if program_path.endswith(".bin") else "txt"
                self.load_program(program_path, type)

    def load_program(self, program_path, type="bin"):
        with open(program_path, "r") as file:
            program_data = file.read()
            if type == "txt":
                program_bytes = bytes.fromhex(program_data)
                self.memory.setDataPartial(list(program_bytes), 0)
            elif type == "bin":
                program_bytes = file.read()
                self.memory.setDataPartial(list(program_bytes), 0)
            print(f"Program loaded from {program_path} as {type}.")
            print(f"Program bytes: {list(program_bytes)}")

#############################################################################################################################################

    def run_steps(self):

        if not self.program_loaded():
            return

        self.cycle += 1
        print("STEP CPU")
        self.cpu.cpuStep()
        self.drain_logs()
        self.cache_dock.update_cache_view()
        self.register_dock.update_register_labels()
        self.pipeline_dock.update_pipeline_view()
        self.memory_dock.update_memory_view()  # Aggiorna la vista della memoria dopo lo step della CPU
        self.shortcut_space_run_steps = QShortcut(QKeySequence("Space"), self)
        self.shortcut_space_run_steps.activated.connect(self.on_space_press)
        return
        

    def run_all(self):

        if not self.program_loaded():
            return
        print("run")
        while self.cycle <= 100:
            self.cycle += 1
            print("STEP CPU")
            self.cpu.cpuStep()
            self.drain_logs()
            self.cache_dock.update_cache_view()
            self.register_dock.update_register_labels()
            self.pipeline_dock.update_pipeline_view()
            self.memory_dock.update_memory_view()  # Aggiorna la vista della memoria dopo lo step della CPU
        
        self.cache.flushAllCaches()  # Flush all caches when the program finishes
        self.cache_dock.update_cache_view()  # Update the cache view after flushing
        self.memory_dock.update_memory_view()  # Update the memory view after flushing
            
            
        return

#########################################################################################################################################

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SimulatorGUI()
    window.show()
    sys.exit(app.exec())

