# -*- coding: utf-8 -*-
import CalibUtils as cu
import cv2 as cv
import numpy as np

class MonoRectification:
    def __init__(self, cameraMatrix, distCoeffs, imageSize):
        self.cameraMatrix = cameraMatrix
        self.distCoeffs = distCoeffs
        self.imageSize = imageSize
        self.alpha = 1
        self.xRatio = 1
        self.yRatio = 1
    
    def computeCorrectionMaps(self, alpha = 1.0):
        newImageSize = (int(self.imageSize[0] / self.xRatio), int(self.imageSize[1] / self.yRatio))
        # Implement getOptimalNewCameraMatrix here
        # Implement initUndistortRectifyMap here
        
    def rectify(self, frame):
        return # Implement remap here
        
    def display(self, cameraId = 0, fps = 25):
        capture = cv.VideoCapture(cameraId)
        
        cv.namedWindow('Rectified Image', cv.WINDOW_NORMAL | cv.WINDOW_GUI_EXPANDED)
        
        # create trackbars
        cv.createTrackbar('10 * alpha', 'Rectified Image', 10 * self.alpha, 10, cu.nullFunction)
        self.computeCorrectionMaps(self.alpha)
        
        while(True):
            if cv.getWindowProperty('Rectified Image',cv.WND_PROP_VISIBLE) < 1:        
                break
            # Capture frame-by-frame
            ret, frame = capture.read()
            
            alpha = cv.getTrackbarPos('10 * alpha', 'Rectified Image') / 10
            
            if alpha != self.alpha:
                self.alpha = alpha
                
                self.computeCorrectionMaps(self.alpha)
                
            frame = self.rectify(frame)
                
            cv.imshow('Rectified Image', frame)           
            
            key = cv.waitKey(int(1000 / fps))

            if key == ord('\x1b') or key == ord('q'):
                break
            
        capture.release()
        cv.destroyAllWindows()
        
class StereoRectification:
    def __init__(self, cameraMatrixLeft, distCoeffsLeft, cameraMatrixRight, distCoeffsRight, imageSize, R, T):
        self.cameraMatrixLeft = cameraMatrixLeft
        self.distCoeffsLeft = distCoeffsLeft
        self.cameraMatrixRight = cameraMatrixRight
        self.distCoeffsRight = distCoeffsRight
        self.imageSize = imageSize
        self.R = R
        self.T = T
        self.alpha = 1
        self.ratio = 1
        self.crop = 0
        self.epipolarLines = 0
    
    def computeCorrectionMaps(self, alpha = 1.0):
        newImageSize = (int(self.imageSize[0] / self.ratio), int(self.imageSize[1] / self.ratio))
        
        # Implement stereoRectify here

        self.mapxLeft, self.mapyLeft = cv.initUndistortRectifyMap(self.cameraMatrixLeft, self.distCoeffsLeft, RLeft, PLeft, newImageSize, cv.CV_32FC1)
        self.mapxRight, self.mapyRight = cv.initUndistortRectifyMap(self.cameraMatrixRight, self.distCoeffsRight, RRight, PRight, newImageSize, cv.CV_32FC1)
        
    def rectify(self, frameLeft, frameRight):
        left = cv.remap(frameLeft, self.mapxLeft, self.mapyLeft, cv.INTER_LINEAR)
        right = cv.remap(frameRight, self.mapxRight, self.mapyRight, cv.INTER_LINEAR)
        return left, right
        
    def display(self, left, right):
        cv.namedWindow('Rectified Stereo Pair', cv.WINDOW_NORMAL | cv.WINDOW_GUI_EXPANDED)
        
        # create trackbars
        cv.createTrackbar('10 * alpha', 'Rectified Stereo Pair', 10 * self.alpha, 10, cu.nullFunction)
        cv.createTrackbar('Display N Epipolar lines', 'Rectified Stereo Pair', self.epipolarLines, 50, cu.nullFunction)
        self.computeCorrectionMaps(self.alpha)
        
        while(True):    
            if cv.getWindowProperty('Rectified Stereo Pair',cv.WND_PROP_VISIBLE) < 1:        
                break
                    
            alpha = cv.getTrackbarPos('10 * alpha', 'Rectified Stereo Pair') / 10
            self.epipolarLines = cv.getTrackbarPos('Display N Epipolar lines', 'Rectified Stereo Pair')
                
            if (alpha != self.alpha):
                self.alpha = alpha
                self.computeCorrectionMaps(self.alpha)
                
            rectLeft, rectRight = self.rectify(left, right)
            display = cv.cvtColor(cv.hconcat([rectLeft, rectRight]), cv.COLOR_GRAY2BGR)
            
            display = cv.resize(display, (0, 0), fx = 0.3, fy = 0.3)
            
            for i in range(0, self.epipolarLines):
                display = cv.line(display, (0, int(i*display.shape[1::-1][1]/self.epipolarLines)), (display.shape[1::-1][0]-1, int(i*display.shape[1::-1][1]/self.epipolarLines)), (0, 0, 200))
            
            cv.imshow('Rectified Stereo Pair', display)           
            
            key = cv.waitKey(1)
            
            if key == ord('\x1b') or key == ord('q'):
                break
            
        cv.destroyAllWindows()
        
    def displayDisparity(self, left, right):
        cv.namedWindow('Disparity', cv.WINDOW_NORMAL | cv.WINDOW_GUI_EXPANDED)
        
        # create trackbars
        cv.createTrackbar('10 * alpha', 'Disparity', int(10 * self.alpha), 10, cu.nullFunction)
        cv.createTrackbar('numDisparities / 16', 'Disparity', 11, 20, cu.nullFunction)
        cv.createTrackbar('(blockSize - 5) / 2', 'Disparity', 8, 20, cu.nullFunction)
        cv.createTrackbar('uniquenessRatio', 'Disparity', 15, 50, cu.nullFunction)
        self.computeCorrectionMaps(self.alpha)
        
        while(True):     
            if cv.getWindowProperty('Disparity',cv.WND_PROP_VISIBLE) < 1:        
                break
                   
            alpha = cv.getTrackbarPos('10 * alpha', 'Disparity') / 10
            numDisparities = 16 * cv.getTrackbarPos('numDisparities / 16', 'Disparity')
            blockSize = 2 * cv.getTrackbarPos('(blockSize - 5) / 2', 'Disparity') + 5
            uniquenessRatio = cv.getTrackbarPos('uniquenessRatio', 'Disparity')
            
            if (alpha != self.alpha):
                self.alpha = alpha
                self.computeCorrectionMaps(self.alpha)
                
            rectLeft, rectRight = self.rectify(left, right)
            
            # Construct the stereo object here (StereoBM_create)
            
            stereo.setMinDisparity(1)
            stereo.setUniquenessRatio(uniquenessRatio)
            
            # Compute the disparity here
            
            minVal, maxVal, minLoc, maxLoc = cv.minMaxLoc(disparity)
            display = cv.convertScaleAbs(disparity, alpha = (255.0 / maxVal - minVal))
            display = cv.cvtColor(display, cv.COLOR_GRAY2BGR)
            display = cv.applyColorMap(display, cv.COLORMAP_JET)
            
            mask = np.copy(disparity)
            mask[np.where(disparity <= [stereo.getNumDisparities() -16])] = [0]
            mask[np.where(disparity > [stereo.getNumDisparities() -16])] = [1]            
            mask = np.uint8(mask)
            display = cv.bitwise_and(display, display, mask = mask)
            
            display = cv.resize(display, (0, 0), fx = 0.3, fy = 0.3)            
            cv.imshow('Disparity', display)           
            
            key = cv.waitKey(1)
            
            if key == ord('\x1b') or key == ord('q'):
                break
            
        cv.destroyAllWindows()
