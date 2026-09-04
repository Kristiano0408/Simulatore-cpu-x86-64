import simulator
from docks import CacheDock, RegisterDock, MemoryDock, PipelineDock
from PySide6.QtWidgets import QApplication,QMainWindow,QWidget
from PySide6.QtGui import QShortcut, QKeySequence
import PySide6.QtCore as QtCore
import sys


DockArea = QtCore.Qt.DockWidgetArea



class SimulatorGUI(QMainWindow):

       
    
    def __init__(self):
        super().__init__()
        self.resize(1200, 800)
        
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
        self.memory.setDataPartial([
            0xB1, 0x22,
            0x48, 0x89, 0x06,       # MOV [RSI], RAX
            0x41, 0xB8, 0x78, 0x56, 0x34, 0x12, 0x00, 0x00, 0x00, 0x00,
        ], 0)

        self.registers.getReg(simulator.Register.RSI).setRaw(0x100)
        self.registers.getReg(simulator.Register.RAX).setRaw(0xCAFEBABE)

        shortcut = QShortcut(QKeySequence("Space"), self)
        shortcut.activated.connect(self.on_space_press)

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


#########################################################################################################################################

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = SimulatorGUI()
    window.show()
    sys.exit(app.exec())

