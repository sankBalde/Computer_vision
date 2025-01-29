#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import cv2 as cv
import numpy as np

def nullFunction(x = None, y = None):
    pass

class Tracker:
    def __init__(self, referenceImage, detectorType = 'ORB'):
        self.detectorTypes = {'ORB':0, 'AKAZE':1}
        
        if type(detectorType) == int:
            assert detectorType in self.detectorTypes.values(), 'Pattern type must be one of {}'.format(self.detectorTypes)
            detectorType = list(self.detectorTypes.keys())[list(self.detectorTypes.values()).index(detectorType)]
        else:
            assert detectorType in self.detectorTypes, 'Pattern type must be one of {}'.format(self.detectorTypes)
            
        self.detectorType = detectorType
        
        self.homography = []
        self.initialized = False
        
        # Create self.detector below
        if self.detectorType == 'ORB':
            # ORB detector
            self.detector = cv.ORB_create()
        elif self.detectorType == 'AKAZE':
            # AKAZE detector
            self.detector = cv.AKAZE_create()
            
        # Reference image
        self.referenceImage = referenceImage
        self.referenceKeypoints, self.referenceDescriptors = self.detector.detectAndCompute(self.referenceImage, None)
        
        self.matcher = cv.DescriptorMatcher_create(cv.DescriptorMatcher_BRUTEFORCE_HAMMING)
        
        # Get the corners from the referenceImage
        size = self.referenceImage.shape
        self.targetCorners = np.array([[[0,0]], 
                                       [[size[1], 0]], 
                                       [[size[1], size[0]]], 
                                       [[0, size[0]]]], dtype=np.float32)   
        
    def detectAndMatch(self, image):
        # Implement feature detection here
        keypoints, descriptors = self.detector.detectAndCompute(image, None)
        if descriptors is None:
            matches = None
        else:
            # Implement feature matching here
            matches = self.matcher.knnMatch(self.referenceDescriptors, descriptors, k=2)
                
        return keypoints, matches
    
    def getCorrespondencePoints(self, keypoints, matches, matchingRatio = 0.7):
        # Filter Matches
        goodPoints = []
        
        if matches is not None:
            # Implement Lowe's ratio test here
            pass
        
        # Get Points        
        targetPoints = np.empty((len(goodPoints),2), dtype=np.float32)    
        observedPoints = np.empty((len(goodPoints),2), dtype=np.float32)
        
        for i in range(len(goodPoints)):
            targetPoints[i,0] = self.referenceKeypoints[goodPoints[i].queryIdx].pt[0]
            targetPoints[i,1] = self.referenceKeypoints[goodPoints[i].queryIdx].pt[1]
            observedPoints[i,0] = keypoints[goodPoints[i].trainIdx].pt[0]
            observedPoints[i,1] = keypoints[goodPoints[i].trainIdx].pt[1]
            
        return goodPoints, targetPoints, observedPoints
    
    def computeHomography(self, targetPoints, observedPoints, minMatches = 12, weight = 0.4):
        homography = []
        
        if len(observedPoints) > minMatches:
            # Find the homography here
            self.homography = homography
                
        else:
            self.homography = []
                    
        return self.homography
    
    def computeObjectCorners(self):
        # Implement perspective transform here
        pass
    
    def drawObjectContours(self, image):
        if len(self.homography) > 0:
            corners = self.computeObjectCorners()
            
            cv.line(image, (int(corners[0,0,0] + self.referenceImage.shape[1]), int(corners[0,0,1])),\
                    (int(corners[1,0,0] + self.referenceImage.shape[1]), int(corners[1,0,1])), (0,255,0), 4)
            cv.line(image, (int(corners[1,0,0] + self.referenceImage.shape[1]), int(corners[1,0,1])),\
                    (int(corners[2,0,0] + self.referenceImage.shape[1]), int(corners[2,0,1])), (0,255,0), 4)
            cv.line(image, (int(corners[2,0,0] + self.referenceImage.shape[1]), int(corners[2,0,1])),\
                    (int(corners[3,0,0] + self.referenceImage.shape[1]), int(corners[3,0,1])), (0,255,0), 4)
            cv.line(image, (int(corners[3,0,0] + self.referenceImage.shape[1]), int(corners[3,0,1])),\
                    (int(corners[0,0,0] + self.referenceImage.shape[1]), int(corners[0,0,1])), (0,255,0), 4)
    
    def display(self, cameraId = 0, minMatches = 12, weight = 0.4, maxframeLost = 5, matchingRatio = 0.8):
        capture = cv.VideoCapture(cameraId)
        cv.namedWindow('Tracked plane')
        
        # create trackbars
        cv.createTrackbar('Matching Ratio (%)', 'Tracked plane', 70, 100, nullFunction)
        cv.createTrackbar('Minimum Matches', 'Tracked plane', 12, 50, nullFunction)
        cv.createTrackbar('Accumulation Weigth (%)', 'Tracked plane', 40, 100, nullFunction)
        
        while(True):
            ret, image = capture.read()
            
            matchingRatio = cv.getTrackbarPos('Matching Ratio (%)', 'Tracked plane') / 100
            minMatches = cv.getTrackbarPos('Minimum Matches', 'Tracked plane')
            weight = cv.getTrackbarPos('Accumulation Weigth (%)', 'Tracked plane') / 100
            
            keypoints, matches = self.detectAndMatch(image)
            goodPoints, targetPoints, observedPoints = self.getCorrespondencePoints(keypoints, matches, matchingRatio)
                
            display = np.empty((max(self.referenceImage.shape[0], image.shape[0]), self.referenceImage.shape[1] + image.shape[1], 3), dtype=np.uint8)
            cv.drawMatches(self.referenceImage, self.referenceKeypoints, image, keypoints, goodPoints, display, flags=cv.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)
                
            # Compute the homography
            
            # Draw the object contours             
            
            cv.imshow('Tracked plane', display)
    
            key = cv.waitKey(1)
            if key == ord('\x1b') or key == ord('q'):
                break
        
        capture.release()
        cv.destroyAllWindows()