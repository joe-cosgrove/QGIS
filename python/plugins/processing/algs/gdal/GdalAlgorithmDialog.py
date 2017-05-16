# -*- coding: utf-8 -*-

"""
***************************************************************************
    GdalAlgorithmDialog.py
    ---------------------
    Date                 : May 2015
    Copyright            : (C) 2015 by Victor Olaya
    Email                : volayaf at gmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
"""

__author__ = 'Victor Olaya'
__date__ = 'May 2015'
__copyright__ = '(C) 2015, Victor Olaya'

# This will get replaced with a git SHA1 when you do a git archive

__revision__ = '$Format:%H$'

from qgis.PyQt.QtWidgets import (QWidget,
                                 QVBoxLayout,
                                 QPushButton,
                                 QLabel,
                                 QPlainTextEdit,
                                 QLineEdit,
                                 QComboBox,
                                 QCheckBox,
                                 QSizePolicy)

from qgis.gui import QgsMessageBar

from processing.gui.AlgorithmDialog import AlgorithmDialog
from processing.gui.AlgorithmDialogBase import AlgorithmDialogBase
from processing.gui.ParametersPanel import ParametersPanel
from processing.gui.MultipleInputPanel import MultipleInputPanel
from processing.gui.NumberInputPanel import NumberInputPanel


class GdalAlgorithmDialog(AlgorithmDialog):

    def __init__(self, alg):
        AlgorithmDialogBase.__init__(self, alg)

        self.alg = alg

        self.bar = QgsMessageBar()
        self.bar.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed)
        self.layout().insertWidget(0, self.bar)

        self.setMainWidget(GdalParametersPanel(self, alg))

        cornerWidget = QWidget()
        layout = QVBoxLayout()
        layout.setContentsMargins(0, 0, 0, 5)
        self.tabWidget.setStyleSheet("QTabBar::tab { height: 30px; }")
        runAsBatchButton = QPushButton(self.tr("Run as batch process..."))
        runAsBatchButton.clicked.connect(self.runAsBatch)
        layout.addWidget(runAsBatchButton)
        cornerWidget.setLayout(layout)
        self.tabWidget.setCornerWidget(cornerWidget)

        self.mainWidget.parametersHaveChanged()


class GdalParametersPanel(ParametersPanel):

    def __init__(self, parent, alg):
        ParametersPanel.__init__(self, parent, alg)

        w = QWidget()
        layout = QVBoxLayout()
        layout.setMargin(0)
        layout.setSpacing(6)
        label = QLabel()
        label.setText(self.tr("GDAL/OGR console call"))
        layout.addWidget(label)
        self.text = QPlainTextEdit()
        self.text.setReadOnly(True)
        layout.addWidget(self.text)
        w.setLayout(layout)
        self.layoutMain.addWidget(w)

        self.connectParameterSignals()
        self.parametersHaveChanged()

    def connectParameterSignals(self):
        for wrapper in list(self.wrappers.values()):
            w = wrapper.widget
            if isinstance(w, QLineEdit):
                w.textChanged.connect(self.parametersHaveChanged)
            elif isinstance(w, QComboBox):
                w.currentIndexChanged.connect(self.parametersHaveChanged)
            elif isinstance(w, QCheckBox):
                w.stateChanged.connect(self.parametersHaveChanged)
            elif isinstance(w, MultipleInputPanel):
                w.selectionChanged.connect(self.parametersHaveChanged)
            elif isinstance(w, NumberInputPanel):
                w.hasChanged.connect(self.parametersHaveChanged)

    def parametersHaveChanged(self):
        try:
            parameters = self.parent.getParamValues()
            for output in self.alg.outputDefinitions():
                if parameters[output.name()] is None:
                    parameters[output.name()] = self.tr("[temporary file]")
            commands = self.alg.getConsoleCommands(parameters)
            commands = [c for c in commands if c not in ['cmd.exe', '/C ']]
            self.text.setPlainText(" ".join(commands))
        except AlgorithmDialogBase.InvalidParameterValue as e:
            self.text.setPlainText(self.tr("Invalid value for parameter '{0}'").format(e.parameter.description()))
        except:
            self.text.setPlainText("")
