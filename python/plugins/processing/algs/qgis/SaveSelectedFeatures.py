# -*- coding: utf-8 -*-

"""
***************************************************************************
    SaveSelectedFeatures.py
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

from qgis.core import (QgsApplication,
                       QgsProcessingUtils)
from processing.core.GeoAlgorithm import GeoAlgorithm
from processing.core.GeoAlgorithmExecutionException import GeoAlgorithmExecutionException
from processing.core.parameters import ParameterVector
from processing.core.outputs import OutputVector


class SaveSelectedFeatures(GeoAlgorithm):

    OUTPUT_LAYER = 'OUTPUT_LAYER'
    INPUT_LAYER = 'INPUT_LAYER'

    def icon(self):
        return QgsApplication.getThemeIcon("/providerQgis.svg")

    def svgIconPath(self):
        return QgsApplication.iconPath("providerQgis.svg")

    def group(self):
        return self.tr('Vector general tools')

    def __init__(self):
        super().__init__()
        self.addParameter(ParameterVector(self.INPUT_LAYER,
                                          self.tr('Input layer')))

        self.addOutput(OutputVector(self.OUTPUT_LAYER,
                                    self.tr('Selection')))

    def name(self):
        return 'saveselectedfeatures'

    def displayName(self):
        return self.tr('Save selected features')

    def processAlgorithm(self, parameters, context, feedback):
        inputFilename = self.getParameterValue(self.INPUT_LAYER)
        output = self.getOutputFromName(self.OUTPUT_LAYER)

        vectorLayer = QgsProcessingUtils.mapLayerFromString(inputFilename, context)

        writer = output.getVectorWriter(vectorLayer.fields(), vectorLayer.wkbType(), vectorLayer.crs(), context)

        features = vectorLayer.getSelectedFeatures()
        count = int(vectorLayer.selectedFeatureCount())
        if count == 0:
            raise GeoAlgorithmExecutionException(self.tr('There are no selected features in the input layer.'))

        total = 100.0 / count
        for current, feat in enumerate(features):
            writer.addFeature(feat)
            feedback.setProgress(int(current * total))
        del writer
