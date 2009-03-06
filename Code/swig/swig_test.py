import unittest
import os
from opencv.highgui import *

class SwigTestCase(unittest.TestCase):
    def testBackgroundSubtraction(self):
        import swistrack
        os.chdir("../../SwisTrackEnvironment")
        core = swistrack.SwisTrackCore("./Components")
        cr = swistrack.ConfigurationReaderXML()
        cr.Open("backgroundSubtraction.swistrack")

	cr.ReadComponents(core)
        for x in cr.mErrorList.mList:
            print "x", x.mMessage
            
	core.TriggerStart();
	core.Start(False)
        i = 0
        while core.IsTriggerActive():
            core.Step()
            image = core.mDataStructureImageBinary.mImage
            print "image", image
            if i > 10:
                cvSaveImage("test.jpg", image)
            i += 1
            
        #subtractor = swistrack.ComponentBackgroundSubtractionCheungKamath(core)
        #from opencv.highgui import cvLoadImage
        #image = cvLoadImage("earth-horizon.jpg")
        #ipImage = cvGetImage(image)
        #core.mDataStructureInput.mImage = image

        #subtractor.OnStart()
        #subtractor.OnStep()
        #subtractor.OnStep()

        
        
if __name__ == "__main__":
    unittest.main()

