import os
import glob
import CalibUtils as cu

from mpl_toolkits.mplot3d import Axes3D 

import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt


### Base Class for calibration implementation
class CalibrationBase:
    def __init__(self, rows = 5, cols = 7, squareSize_m = 1.0, patternType = 'chessboard'):
        self.patternTypes = {'chessboard':0, 'asymmetric_circles':1}
        
        if type(patternType) == int:
            assert patternType in self.patternTypes.values(), 'Pattern type must be one of {}'.format(self.patternTypes)
            patternType = list(self.patternTypes.keys())[list(self.patternTypes.values()).index(patternType)]
        else:
            assert patternType in self.patternTypes, 'Pattern type must be one of {}'.format(self.patternTypes)
        
        self.cols = cols
        self.rows = rows
        self.squareSize = squareSize_m
        self.patternType = patternType
        self.baseline = 0
        self.criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

    def symmetricWorldPoints(self):
        patternPoints = []
        
        for i in range(self.rows):
            for j in range(self.cols):
                patternPoints.append((j * self.squareSize, i * self.squareSize, 0))
                
        patternPoints = np.array(patternPoints).astype('float32')
                
        return patternPoints
        
    def asymmetricWorldPoints(self):
        patternPoints = []
        
        for i in range(self.rows):
            for j in range(self.cols):
                patternPoints.append(((2*j + i % 2) * self.squareSize, i * self.squareSize, 0))
        
        patternPoints = np.array(patternPoints).astype('float32')
                
        return patternPoints
    
    def detect(self, frame, refine = False):    
        # Find the chessboard corners or circle grid centers
        patternFound = False
        corners = []
                  
        if self.patternType == 'chessboard':
            # Implement findChessboardCorners here
            patternFound, corners = cv.findChessboardCorners(frame, (self.cols, self.rows),  flags=cv.CALIB_CB_ADAPTIVE_THRESH + cv.CALIB_CB_FAST_CHECK + cv.CALIB_CB_NORMALIZE_IMAGE)
            
            if patternFound and refine:
                # Implement cornerSubPix here
                corners = cv.cornerSubPix(frame, corners, (11, 11), (-1, -1), self.criteria)

        elif self.patternType == 'asymmetric_circles':
            # Implement findCirclesGrid here
            patternFound, corners = cv.findCirclesGrid(
                frame, (self.cols, self.rows),
                flags=cv.CALIB_CB_ASYMMETRIC_GRID
            )
            """if corners is not None:
                patternFound = True"""


        return patternFound, corners
        
    def drawPattern(self, frame, corners, patternFound):
        return cv.drawChessboardCorners(frame.copy(), (self.cols, self.rows), corners, patternFound)
    
    def detectInImages(self, framesPath = 'results/acquired', outdir = 'results/calibration'):
        if not os.path.exists(outdir):
            os.makedirs(outdir)
            
        framesPathList = []
        framesPathList.extend(glob.glob(os.path.join(framesPath, '*.png')))
        framesPathList.sort()
        print('There are {} {} images given for calibration'.format(len(framesPathList),self.patternType))
        
        # Arrays to store object points and image points from all the selected images.
        objectPoints = [] # 3d point in real world space
        imgPoints = [] # 2d points in image plane.
        workingImages = []
        
        if self.patternType in ['chessboard', 'symmetric_circles']:
            patternPoints = self.symmetricWorldPoints()
        elif self.patternType == 'asymmetric_circles':
            patternPoints = self.asymmetricWorldPoints()
            
        for framePath in framesPathList:
            print('Processing {}'.format(framePath))
            
            frame = cv.imread(framePath, cv.IMREAD_GRAYSCALE)
            
            if frame is None:
                print('Failed to load {}'.format(framePath))
                return None
                    
            patternFound, corners = self.detect(frame, True)
       
            if patternFound:
                objectPoints.append(patternPoints)
                imgPoints.append(corners)
                workingImages.append(os.path.basename(framePath))
            
                drawToFrame = cv.cvtColor(frame, cv.COLOR_GRAY2BGR)
                drawToFrame = self.drawPattern(drawToFrame, corners, patternFound)
                cv.imwrite(outdir + '/' + os.path.basename(framePath) , drawToFrame)
                    
            else:
                print('Calibration board NOT FOUND')
        
        return workingImages, objectPoints, imgPoints, frame.shape[1::-1]
    
    def plotRMS(self, figureName = 'RMS Plot'):
        # Plot settings
        plt.figure(figureName)
        plt.title(figureName)
        plt.xlabel('Image ID')
        plt.ylabel('RMS')
        
        x = [os.path.splitext(os.path.basename(image))[0] for image in self.workingImages]
        
        if len(self.perViewErrors[0]) == 2:
            plt.scatter(x, [rms[0] for rms in self.perViewErrors], label='Per image RMS (Left Camera)', marker='o')
            plt.scatter(x, [rms[1] for rms in self.perViewErrors], label='Per image RMS (Right Camera)', marker='o')
        else:
            plt.scatter(x, self.perViewErrors, label='Per image RMS', marker='o')
        
        plt.plot(x, [self.rms]*len(self.workingImages), label='Mean RMS', linestyle='--')
        plt.legend(loc='upper right')
        
        if not os.path.exists('results/analysis'):
            os.makedirs('results/analysis')
    
        plt.savefig('results/analysis/'+ figureName + '.png')
        plt.show()
        
    def visualizeBoards(self, scale = 0.00002):       
        figureName = 'Calibration boards visualization in camera frame'
        
        # Plot settings
        fig = plt.figure(figureName)
        ax = fig.add_subplot(projection = '3d')
        ax.set_title(figureName)
        ax.set_xlabel('x')
        ax.set_ylabel('z')
        ax.set_zlabel('-y')
        
        try:
            cu.plot_camera_frame(ax, self.rvecs, self.tvecs, self.imageSize[0]*scale, self.imageSize[1]*scale, self.baseline, f=self.cameraMatrix[0][0]*scale)
        except AttributeError:
            cu.plot_camera_frame(ax, self.rvecs, self.tvecs, self.imageSize[0]*scale, self.imageSize[1]*scale, self.baseline, f=self.cameraMatrixLeft[0][0]*scale)
                        
        cu.plot_board_frames(ax, self.rvecs, self.tvecs, self.cols, self.rows, self.squareSize)
        cu.set_axes_equal(ax) 
        
        if not os.path.exists('results/analysis'):
            os.makedirs('results/analysis')
        
        plt.savefig('results/analysis/' + figureName + '.png')
        plt.show()
        
### Monocular Calibration class     
class MonoCalibration(CalibrationBase):      
    def acquire(self, cameraId = 0, startIndex = 0):
        if not os.path.exists('results/acquired'):
            os.makedirs('results/acquired')
            
        imgIndex = startIndex
        capture = cv.VideoCapture(cameraId)
        
        cv.namedWindow('Camera', cv.WINDOW_NORMAL | cv.WINDOW_GUI_EXPANDED)
        patternTypeName = '0 : Checkerboard \n1 : Asymetric circles grid'
        
        # create trackbars
        cv.createTrackbar('Cols', 'Camera', self.cols, 20, cu.nullFunction)
        cv.createTrackbar('Rows', 'Camera', self.rows, 20, cu.nullFunction)
        cv.createTrackbar('Pattern size (mm)', 'Camera', int(self.squareSize * 1000), 100, cu.nullFunction)
        cv.createTrackbar(patternTypeName, 'Camera', self.patternTypes[self.patternType], 1, cu.nullFunction)
        
        while True:
            #if cv.getWindowProperty('Camera',cv.WND_PROP_VISIBLE) < 1:
                #break
        
            # Capture frame-by-frame
            ret, frame = capture.read()

            # Find and draw the pattern on the frane
            gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
            patternFound, corners = self.detect(gray)
            drawToFrame = self.drawPattern(frame, corners, patternFound)
            
            # Flip the display to ease the user's movements
            drawToFrame = cv.flip(drawToFrame, 1)

            # Display the resulting frame
            cv.imshow('Camera', drawToFrame)
            self.cols = cv.getTrackbarPos('Cols', 'Camera')
            self.rows = cv.getTrackbarPos('Rows', 'Camera')
            self.squareSize = cv.getTrackbarPos('Square size (mm)', 'Camera') / 1000
            self.patternType = list(self.patternTypes.keys())[list(self.patternTypes.values()).index(cv.getTrackbarPos(patternTypeName, 'Camera'))]            
        
            key = cv.waitKey(100)

            if key == ord('\x1b') or key == ord('q'):
                break
            elif key == ord(' ') or key == ord('\r'):
                if patternFound:
                    writeName = 'results/acquired/' + str(imgIndex) + '.png'
                    print('Saving image ' + str(imgIndex) + ' as ' + writeName)
                    cv.imwrite(writeName, cv.cvtColor(frame, cv.COLOR_BGR2GRAY))
                    imgIndex += 1
                else:
                    print('Could not find pattern, image not saved')
            
        capture.release()
        cv.destroyAllWindows()
        
    def calibrate(self, framesPath = 'results/acquired'):
        self.workingImages, objectPoints, imgPoints, self.imageSize = self.detectInImages(framesPath)
  
        # Implement calibrateCameraExtended here
        ret = cv.calibrateCameraExtended(objectPoints, imgPoints, self.imageSize, None, None, criteria=self.criteria)
        self.rms = ret[0]
        self.cameraMatrix = ret[1]
        self.distCoeffs = ret[2]
        self.rvecs = ret[3]
        self.tvecs = ret[4]
        self.perViewErrors = ret[-1]
        
        print('\nRMS:', self.rms)
        print('camera matrix:\n', self.cameraMatrix)
        print('distortion coefficients: ', self.distCoeffs.ravel())
        
        return self.rms, self.cameraMatrix, self.distCoeffs, self.imageSize

### Stereo Calibration Class        
class StereoCalibration(CalibrationBase):        
    def calibrate(self, framesPathLeft = 'data/stereo/calibration/left', framesPathRight  = 'data/stereo/calibration/right'):        
        workingImagesLeft, objectPointsLeft, imgPointsLeft, self.imageSize = self.detectInImages(framesPathLeft, 'results/calibration/left')
        workingImagesRight, objectPointsRight, imgPointsRight, _ = self.detectInImages(framesPathRight, 'results/calibration/right')
        self.workingImages = workingImagesLeft
        
        # Remove images for which the board was only detected by one camera        
        if workingImagesLeft != workingImagesRight:
            for image in workingImagesLeft:
                if workingImagesRight.count(image) != 1:
                    objectPointsLeft.pop(workingImagesLeft.index(image))
                    imgPointsLeft.pop(workingImagesLeft.index(image))
                    self.workingImages.remove(image)
                    
            for image in workingImagesRight:
                if workingImagesLeft.count(image) != 1:
                    print(workingImagesRight.index(image))
                    print(len(objectPointsRight))
                    objectPointsRight.pop(workingImagesRight.index(image)-1)
                    imgPointsRight.pop(workingImagesRight.index(image)-1)
                    self.workingImages.remove(image)
                
        # Implement stereoCalibrateExtended here
        
        self.baseline = self.T.ravel()[0]
        print('\nRMS:', self.rms)
        print('Left camera matrix:\n', self.cameraMatrixLeft)
        print('Left distortion coefficients: ', self.distCoeffsLeft.ravel())
        
        print('\nRight camera matrix:\n', self.cameraMatrixRight)
        print('Right distortion coefficients: ', self.distCoeffsRight.ravel())
        
        print('\nRotation matrix:\n', self.R)
        print('\nTranslation: ', self.T.ravel())
        
        return self.rms, self.cameraMatrixLeft, self.distCoeffsLeft, self.cameraMatrixRight, self.distCoeffsRight, self.imageSize, self.R, self.T
