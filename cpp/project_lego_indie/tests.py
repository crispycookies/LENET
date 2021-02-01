import numpy as np
import os
import cv2

def drawlines(lines_edges, img): 
    for i in range(0, len(lines_edges)): 
        rho = lines_edges[i][0][0] 
        theta = lines_edges[i][0][1] # calc x,y from rho and theta 
        a = np.cos(theta) 
        b = np.sin(theta) 
        x0 = a * rho 
        y0 = b * rho 
        x1 = int(x0 + 1000 * (-b)) 
        y1 = int(y0 + 1000 * (a)) 
        x2 = int(x0 - 1000 * (-b)) 
        y2 = int(y0 - 1000 * (a)) 
        cv2.line(img, (x1, y1), (x2, y2), (0, 0, 255), 2)
        
def drawlinesP(lines_edges, img): 
    for line in lines_edges: 
        x1, y1, x2, y2 = line[0] 
        cv2.line(img, (x1, y1), (x2, y2), (0, 0, 255), 2) 
        
def showimg(img): 
    cv2.imshow("", img) 
    cv2.waitKey() 
    cv2.destroyAllWindows() 
    
bg = cv2.imread("pic/All/image_87.jpg") 

bg[bg==0] = 1 
mul = np.ones(bg.shape) * 255 


for root, dirs, files in os.walk("pic/All"):
    for filename in files:
        img = cv2.imread("pic/All/" + filename, cv2.IMREAD_COLOR)
        bcorr = img / bg 
        bcorr = cv2.multiply(bcorr, mul, dtype=cv2.CV_8U)
        grey = cv2.cvtColor(bcorr, cv2.COLOR_BGR2GRAY)
        edges = cv2.Sobel(grey, cv2.CV_8U, 0, 1)
        #edges = cv2.Canny(grey, 50, 150, apertureSize = 3)
        #edges = cv2.Laplacian(grey, cv2.CV_8U,7)
        edges_thresh = cv2.threshold(edges, 127, 255, cv2.THRESH_BINARY|cv2.THRESH_OTSU)[1]
        lines = cv2.HoughLinesP(edges_thresh, 1, np.pi/180, 50, 100, 10)
        drawlinesP(lines, img)
        cv2.imshow("edges", edges)
        cv2.imshow("edges_thresh", edges_thresh)
        cv2.imshow("image", img)
        cv2.waitKey()
        cv2.destroyAllWindows()
