# -*- coding: utf-8 -*-

"""
***************************************************************************
    ModelerScene.py
    ---------------------
    Date                 : August 2012
    Copyright            : (C) 2012 by Victor Olaya
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
__date__ = 'August 2012'
__copyright__ = '(C) 2012, Victor Olaya'

# This will get replaced with a git SHA1 when you do a git archive

__revision__ = '$Format:%H$'

from qgis.PyQt.QtCore import QPointF, Qt
from qgis.PyQt.QtWidgets import QGraphicsItem, QGraphicsScene
from qgis.core import QgsProcessingParameterDefinition
from processing.modeler.ModelerGraphicItem import ModelerGraphicItem
from processing.modeler.ModelerArrowItem import ModelerArrowItem
from processing.modeler.ModelerAlgorithm import ValueFromInput, ValueFromOutput, CompoundValue


class ModelerScene(QGraphicsScene):

    def __init__(self, parent=None):
        super(ModelerScene, self).__init__(parent)
        self.paramItems = {}
        self.algItems = {}
        self.outputItems = {}
        self.setItemIndexMethod(QGraphicsScene.NoIndex)

    def getParameterPositions(self):
        return {key: item.pos() for key, item in list(self.paramItems.items())}

    def getAlgorithmPositions(self):
        return {key: item.pos() for key, item in list(self.algItems.items())}

    def getOutputPositions(self):
        pos = {}
        for algName, outputs in list(self.outputItems.items()):
            outputPos = {}
            for (key, value) in list(outputs.items()):
                if value is not None:
                    outputPos[key] = value.pos()
                else:
                    outputPos[key] = None
            pos[algName] = outputPos
        return pos

    def getItemsFromParamValue(self, value):
        items = []
        if isinstance(value, list):
            for v in value:
                items.extend(self.getItemsFromParamValue(v))
        elif isinstance(value, CompoundValue):
            for v in value.values:
                items.extend(self.getItemsFromParamValue(v))
        elif isinstance(value, ValueFromInput):
            items.append((self.paramItems[value.name], 0))
        elif isinstance(value, ValueFromOutput):
            outputs = self.model.algs[value.alg].algorithm.outputDefinitions()
            for i, out in enumerate(outputs):
                if out.name() == value.output:
                    break
            if value.alg in self.algItems:
                items.append((self.algItems[value.alg], i))
        return items

    def paintModel(self, model, controls=True):
        self.model = model
        # Inputs
        for inp in list(model.inputs.values()):
            item = ModelerGraphicItem(inp, model, controls)
            item.setFlag(QGraphicsItem.ItemIsMovable, True)
            item.setFlag(QGraphicsItem.ItemIsSelectable, True)
            self.addItem(item)
            item.setPos(inp.pos.x(), inp.pos.y())
            self.paramItems[inp.param.name()] = item

        # We add the algs
        for alg in list(model.algs.values()):
            item = ModelerGraphicItem(alg, model, controls)
            item.setFlag(QGraphicsItem.ItemIsMovable, True)
            item.setFlag(QGraphicsItem.ItemIsSelectable, True)
            self.addItem(item)
            item.setPos(alg.pos.x(), alg.pos.y())
            self.algItems[alg.childId()] = item

        # And then the arrows
        for alg in list(model.algs.values()):
            idx = 0
            for parameter in alg.algorithm.parameterDefinitions():
                if not parameter.isDestination() and not parameter.flags() & QgsProcessingParameterDefinition.FlagHidden:
                    if parameter.name() in alg.params:
                        value = alg.params[parameter.name()]
                    else:
                        value = None
                    sourceItems = self.getItemsFromParamValue(value)
                    for sourceItem, sourceIdx in sourceItems:
                        arrow = ModelerArrowItem(sourceItem, sourceIdx, self.algItems[alg.childId()], idx)
                        sourceItem.addArrow(arrow)
                        self.algItems[alg.childId()].addArrow(arrow)
                        arrow.updatePath()
                        self.addItem(arrow)
                    idx += 1
            for depend in alg.dependencies():
                arrow = ModelerArrowItem(self.algItems[depend], -1,
                                         self.algItems[alg.childId()], -1)
                self.algItems[depend].addArrow(arrow)
                self.algItems[alg.childId()].addArrow(arrow)
                arrow.updatePath()
                self.addItem(arrow)

        # And finally the outputs
        for alg in list(model.algs.values()):
            outputs = alg.outputs
            outputItems = {}
            idx = 0
            for key in outputs:
                out = outputs[key]
                if out is not None:
                    item = ModelerGraphicItem(out, model, controls)
                    item.setFlag(QGraphicsItem.ItemIsMovable, True)
                    item.setFlag(QGraphicsItem.ItemIsSelectable, True)
                    self.addItem(item)
                    pos = alg.outputs[key].pos
                    if pos is None:
                        pos = (alg.position() + QPointF(ModelerGraphicItem.BOX_WIDTH, 0) +
                               self.algItems[alg.childId()].getLinkPointForOutput(idx))
                    item.setPos(pos)
                    outputItems[key] = item
                    arrow = ModelerArrowItem(self.algItems[alg.childId()], idx, item,
                                             -1)
                    self.algItems[alg.childId()].addArrow(arrow)
                    item.addArrow(arrow)
                    arrow.updatePath()
                    self.addItem(arrow)
                    idx += 1
                else:
                    outputItems[key] = None
            self.outputItems[alg.childId()] = outputItems

    def mousePressEvent(self, mouseEvent):
        if mouseEvent.button() != Qt.LeftButton:
            return
        super(ModelerScene, self).mousePressEvent(mouseEvent)
