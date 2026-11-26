from PySide6.QtWidgets import QApplication,QMainWindow,QWidget,QMenuBar
from PySide6.QtGui import QShortcut, QKeySequence, QAction




class MenuBar(QMenuBar):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.file_menu =self.addMenu("File")
        self.open_file_action = QAction("Open Program", self)
        self.save_stats_to_json_action = QAction("Save Stats", self)
        self.file_menu.addAction(self.open_file_action)
        self.file_menu.addAction(self.save_stats_to_json_action)

        self.view_menu = self.addMenu("View")

        self.run_menu = self.addMenu("Run")
        self.run_steps_action = QAction("Run Steps", self)
        self.run_all_action = QAction("Run All", self)
        self.run_menu.addAction(self.run_steps_action)  
        self.run_menu.addAction(self.run_all_action)



        