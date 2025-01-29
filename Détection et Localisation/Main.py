#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from __future__ import print_function
import cv2 as cv
import numpy as np
from math import sqrt

from Homography import PerspectiveCorrection
from Tracking import Tracker
from PoseEstimation import PoseEstimator

# Dimension of the object in the real world in meters
objectSize = (0, 0, 0)

scale = 2000
outImageSize = (int(scale * objectSize[0]), int(scale * objectSize[1]))

# Read the image of your object
image = cv.imread("data/img.png")

### Perspective correction
# Compute the homography and save your undistorted image


### Detection & Tracking
# Read the image obtained in the first part
img_object = cv.imread("") 

# Detect and track the object


### Localization
# Webcam intrinsic parameters obtained in TP1 Perception3D
cameraMatrix = np.array([ [ 1, 0, 0 ],
                          [ 0, 1, 0 ],
                          [ 0, 0, 1 ]], np.float64)
distCoeffs = np.array([0, 0, 0, 0, 0], np.float64)

# Estimate the pose and reproject on the image
