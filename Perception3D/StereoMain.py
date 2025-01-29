# -*- coding: utf-8 -*-
from Calibration import StereoCalibration
from Rectification import StereoRectification
import cv2 as cv
import numpy as np


# Calibration

# Visualization

# Rectification

# 3D reconstruction
left = cv.imread('data/stereo/MinnieRawLeft.png', cv.IMREAD_GRAYSCALE)
right = cv.imread('data/stereo/MinnieRawRight.png', cv.IMREAD_GRAYSCALE)
            
