# -*- coding: utf-8 -*-

"""
***************************************************************************
    SelectByLocation.py
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

import os

from qgis.PyQt.QtXml import QDomDocument

from qgis.core import (QgsApplication,
                       QgsProcessingUtils)
from processing.core.GeoAlgorithm import GeoAlgorithm
from processing.core.parameters import ParameterFile
from processing.core.parameters import ParameterRaster
from processing.core.outputs import OutputRaster
from processing.tools import dataobjects
from qgis.utils import iface


class SetRasterStyle(GeoAlgorithm):

    INPUT = 'INPUT'
    STYLE = 'STYLE'
    OUTPUT = 'OUTPUT'

    def icon(self):
        return QgsApplication.getThemeIcon("/providerQgis.svg")

    def svgIconPath(self):
        return QgsApplication.iconPath("providerQgis.svg")

    def group(self):
        return self.tr('Raster general tools')

    def name(self):
        return 'setstyleforrasterlayer'

    def displayName(self):
        return self.tr('Set style for raster layer')

    def defineCharacteristics(self):
        self.addParameter(ParameterRaster(self.INPUT,
                                          self.tr('Raster layer')))
        self.addParameter(ParameterFile(self.STYLE,
                                        self.tr('Style file'), False, False, 'qml'))
        self.addOutput(OutputRaster(self.OUTPUT, self.tr('Styled'), True))

    def processAlgorithm(self, context, feedback):
        filename = self.getParameterValue(self.INPUT)
        layer = QgsProcessingUtils.mapLayerFromString(filename, context)

        style = self.getParameterValue(self.STYLE)
        if layer is None:
            dataobjects.load(filename, os.path.basename(filename), style=style)
            self.getOutputFromName(self.OUTPUT).open = False
        else:
            with open(style) as f:
                xml = "".join(f.readlines())
            d = QDomDocument()
            d.setContent(xml)
            n = d.firstChild()
            layer.readSymbology(n, '')
            self.setOutputValue(self.OUTPUT, filename)
            iface.mapCanvas().refresh()
