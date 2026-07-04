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

        self.setCentralWidget(QWidget())
        self.register_dock = RegisterDock(self)
        self.cache_dock = CacheDock(self)
        self.pipeline_dock = PipelineDock(self)
        self.memory_dock = MemoryDock(self)
        self.addDockWidget(DockArea.TopDockWidgetArea, self.register_dock)
        self.addDockWidget(DockArea.LeftDockWidgetArea, self.memory_dock)
        self.addDockWidget(DockArea.RightDockWidgetArea, self.cache_dock)
        self.addDockWidget(DockArea.BottomDockWidgetArea, self.pipeline_dock)

        

        self.memory.setDataPartial([
            0x2C, 0x03,
            0x2D, 0x05,0x00,0x00,0x00,
            0x80, 0xEB, 0x01,
            0x81, 0xE9, 0xFF,0xFF,0xFF,0xFF,
            0x83, 0xEA, 0x02
        ], 0)

        shortcut = QShortcut(QKeySequence("Space"), self)
        shortcut.activated.connect(self.on_space_press)

    def on_space_press(self):
        print("STEP CPU")
        self.cpu.cpuStep()
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
