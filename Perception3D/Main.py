#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from __future__ import print_function
import cv2 as cv
import numpy as np
from Calibration import MonoCalibration
from math import sqrt

from Homography import PerspectiveCorrection
from Tracking import Tracker
from PoseEstimation import PoseEstimator

# Dimension of the object in the real world in meters
objectSize = (0, 0, 0)
scale = 3


calibration = MonoCalibration(rows=11, cols=4, squareSize_m=0.015, patternType='asymmetric_circles')
rms, cameraMatrix, distCoeffs, imageSize = calibration.calibrate()

### Perspective correction
# Compute the homography and save your undistorted image



### Detection & Tracking
img_object = cv.imread("data/iphone_sam_im.png")
new_dim = (int(img_object.shape[1] / scale), int(img_object.shape[0] / scale))
img_object = cv.resize(img_object, new_dim)
# Detect and track the object
tracker = Tracker(img_object)
#tracker.display()


### Localization
# Dimension of the object in the real world
obj_height = 0.15
obj_width = 0.075
obj_depth = 0.013
objectSize = (obj_width,obj_height , obj_depth)

# Estimate the pose and reproject on the image
posEst = PoseEstimator(img_object, objectSize, cameraMatrix, distCoeffs, tracker.detectorType)
posEst.display()
# Webcam intrinsic parameters obtained in TP1 Perception3D
#cameraMatrix = np.array([ [ 1, 0, 0 ],[ 0, 1, 0 ],[ 0, 0, 1 ]], np.float64)
#distCoeffs = np.array([0, 0, 0, 0, 0], np.float64)

# Estimate the pose and reproject on the image
