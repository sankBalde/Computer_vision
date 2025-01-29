#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cv2 as cv
import numpy as np
from Tracking import Tracker

def nullFunction(x = None, y = None):
    pass

class PoseEstimator(Tracker):
    def __init__(self, referenceImage, objectSize, cameraMatrix, distCoeffs, detectorType = 'ORB'):
        """ Init
        
        :param referenceImage: Reference image to be tracked
        :param objectSize: Size of the real object, (length, width, depth)
        :param detectorType: Type of the detector (ORB or AKAZE)
        """
        Tracker.__init__(self, referenceImage, detectorType)
        
        self.cameraMatrix = cameraMatrix
        self.distCoeffs = distCoeffs
        
        # Add box points here

        self.boxPoints = np.float32([[0, 0, 0],
                                     [objectSize[0], 0, 0],
                                     [objectSize[0], objectSize[1], 0],
                                     [0, objectSize[1], 0],
                                     [0, 0, objectSize[2]],
                                     [objectSize[0], 0, objectSize[2]],
                                     [objectSize[0], objectSize[1], objectSize[2]],
                                     [0, objectSize[1], objectSize[2]]])

        self.objectCorners = np.array([[self.boxPoints[0]],
                                       [self.boxPoints[1]], 
                                       [self.boxPoints[2]], 
                                       [self.boxPoints[3]]], dtype=np.float32)
                                       
    def computePose(self):
        rvecs = []
        tvecs = []
        
        if self.homography is not None:
            if len(self.homography) > 0:
                # Compute corners with perspectiveTransform
                dst = self.computeObjectCorners()
                #dst = cv.perspectiveTransform(self.targetCorners, self.homography)

                # Solve the PnP problem
                success, rvecs, tvecs = cv.solvePnP(self.objectCorners, dst, self.cameraMatrix, self.distCoeffs)

        return rvecs, tvecs
    
    def drawObjectBox(self, image, rvecs, tvecs):
        if len(rvecs) == 3:
            # Implement projectPoints here         
            imagePoints, _ = cv.projectPoints(self.boxPoints, rvecs, tvecs, self.cameraMatrix, self.distCoeffs)
            imagePoints = np.int32(imagePoints).reshape(-1,2)
            image = cv.drawContours(image, [imagePoints[:4]],-1,(0,255,0),3)
            
            for i,j in zip(range(4),range(4,8)):
                image = cv.line(image, tuple(imagePoints[i]), tuple(imagePoints[j]),(255),3)
    
            image = cv.drawContours(image, [imagePoints[4:]],-1,(0,0,255),3)
        
        return image
    
    def display(self, cameraId = 0):
        capture = cv.VideoCapture(cameraId)
        cv.namedWindow('Pose estimation')
        
        # create trackbars
        cv.createTrackbar('Matching Ratio (%)', 'Pose estimation', 70, 100, nullFunction)
        cv.createTrackbar('Minimum Matches', 'Pose estimation', 12, 50, nullFunction)
        cv.createTrackbar('Accumulation Weigth (%)', 'Pose estimation', 40, 100, nullFunction)
        
        while(True):
            ret, image = capture.read()
            
            matchingRatio = cv.getTrackbarPos('Matching Ratio (%)', 'Pose estimation') / 100
            minMatches = cv.getTrackbarPos('Minimum Matches', 'Pose estimation')
            weight = cv.getTrackbarPos('Accumulation Weigth (%)', 'Pose estimation') / 100
            
            keypoints, matches = self.detectAndMatch(image)
            goodPoints, targetPoints, observedPoints = self.getCorrespondencePoints(keypoints, matches, matchingRatio)
            self.computeHomography(targetPoints, observedPoints, minMatches, weight)
            
            rvecs, tvecs = self.computePose()
            image = self.drawObjectBox(image, rvecs, tvecs)
                            
            cv.imshow('Pose estimation', cv.flip(image, 1))
    
            key = cv.waitKey(1)
            if key == ord('\x1b') or key == ord('q'):
                break
        
        capture.release()
        cv.destroyAllWindows()