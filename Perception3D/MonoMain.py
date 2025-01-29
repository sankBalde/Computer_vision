# -*- coding: utf-8 -*-
from Calibration import MonoCalibration
from Rectification import MonoRectification


# Acquisition
calibration = MonoCalibration(rows=11, cols=4, squareSize_m=0.015, patternType='asymmetric_circles')


# Acquisition des images
#calibration.acquire()

# Calibration
calibration.calibrate()

# Visualization
#calibration.visualizeBoards()
#calibration.plotRMS()
# Rectification

