# -*- coding: utf-8 -*-

"""
***************************************************************************
    RandomPointsLayer.py
    ---------------------
    Date                 : April 2014
    Copyright            : (C) 2014 by Alexander Bruy
    Email                : alexander dot bruy at gmail dot com
***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************
"""

__author__ = 'Alexander Bruy'
__date__ = 'April 2014'
__copyright__ = '(C) 2014, Alexander Bruy'

# This will get replaced with a git SHA1 when you do a git archive

__revision__ = '$Format:%H$'

import os
import random

from qgis.PyQt.QtGui import QIcon
from qgis.PyQt.QtCore import QVariant
from qgis.core import (QgsGeometry, QgsFeatureSink, QgsFields, QgsField, QgsSpatialIndex, QgsWkbTypes,
                       QgsPointXY, QgsFeature, QgsFeatureRequest,
                       QgsMessageLog,
                       QgsProcessingUtils)

from processing.algs.qgis.QgisAlgorithm import QgisAlgorithm
from processing.core.parameters import ParameterVector
from processing.core.parameters import ParameterNumber
from processing.core.outputs import OutputVector
from processing.tools import dataobjects, vector

pluginPath = os.path.split(os.path.split(os.path.dirname(__file__))[0])[0]


class RandomPointsLayer(QgisAlgorithm):

    VECTOR = 'VECTOR'
    POINT_NUMBER = 'POINT_NUMBER'
    MIN_DISTANCE = 'MIN_DISTANCE'
    OUTPUT = 'OUTPUT'

    def icon(self):
        return QIcon(os.path.join(pluginPath, 'images', 'ftools', 'random_points.png'))

    def group(self):
        return self.tr('Vector creation tools')

    def __init__(self):
        super().__init__()
        self.addParameter(ParameterVector(self.VECTOR,
                                          self.tr('Input layer'), [dataobjects.TYPE_VECTOR_POLYGON]))
        self.addParameter(ParameterNumber(self.POINT_NUMBER,
                                          self.tr('Points number'), 1, None, 1))
        self.addParameter(ParameterNumber(self.MIN_DISTANCE,
                                          self.tr('Minimum distance'), 0.0, None, 0.0))
        self.addOutput(OutputVector(self.OUTPUT, self.tr('Random points'), datatype=[dataobjects.TYPE_VECTOR_POINT]))

    def name(self):
        return 'randompointsinlayerbounds'

    def displayName(self):
        return self.tr('Random points in layer bounds')

    def processAlgorithm(self, parameters, context, feedback):
        layer = QgsProcessingUtils.mapLayerFromString(self.getParameterValue(self.VECTOR), context)
        pointCount = int(self.getParameterValue(self.POINT_NUMBER))
        minDistance = float(self.getParameterValue(self.MIN_DISTANCE))

        bbox = layer.extent()
        idxLayer = QgsProcessingUtils.createSpatialIndex(layer, context)

        fields = QgsFields()
        fields.append(QgsField('id', QVariant.Int, '', 10, 0))
        writer = self.getOutputFromName(self.OUTPUT).getVectorWriter(fields, QgsWkbTypes.Point, layer.crs(), context)

        nPoints = 0
        nIterations = 0
        maxIterations = pointCount * 200
        total = 100.0 / pointCount if pointCount else 1

        index = QgsSpatialIndex()
        points = dict()

        random.seed()

        while nIterations < maxIterations and nPoints < pointCount:
            rx = bbox.xMinimum() + bbox.width() * random.random()
            ry = bbox.yMinimum() + bbox.height() * random.random()

            pnt = QgsPointXY(rx, ry)
            geom = QgsGeometry.fromPoint(pnt)
            ids = idxLayer.intersects(geom.buffer(5, 5).boundingBox())
            if len(ids) > 0 and \
                    vector.checkMinDistance(pnt, index, minDistance, points):
                request = QgsFeatureRequest().setFilterFids(ids).setSubsetOfAttributes([])
                for f in layer.getFeatures(request):
                    tmpGeom = f.geometry()
                    if geom.within(tmpGeom):
                        f = QgsFeature(nPoints)
                        f.initAttributes(1)
                        f.setFields(fields)
                        f.setAttribute('id', nPoints)
                        f.setGeometry(geom)
                        writer.addFeature(f, QgsFeatureSink.FastInsert)
                        index.insertFeature(f)
                        points[nPoints] = pnt
                        nPoints += 1
                        feedback.setProgress(int(nPoints * total))
            nIterations += 1

        if nPoints < pointCount:
            QgsMessageLog.logMessage(self.tr('Can not generate requested number of random points. '
                                             'Maximum number of attempts exceeded.'), self.tr('Processing'), QgsMessageLog.INFO)

        del writer
