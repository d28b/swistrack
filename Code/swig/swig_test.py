import unittest
import os
from opencv.highgui import *

class SwigTestCase(unittest.TestCase):
    def testBackgroundSubtraction(self):
        import swistrack
        core = swistrack.SwisTrackCore("/home/stefie10/dev/cogmac/tfs/slimd/3rdParty/swistrack/swistrack/SwisTrackEnvironment/Components/")
        cr = swistrack.ConfigurationReaderXML()
        if not cr.Open("backgroundSubtraction.swistrack"):
            print "Couldn't read file."
            self.fail()

	cr.ReadComponents(core)
        for x in cr.mErrorList.mList:
            print "x", x.mMessage
        print "running with", len(core.GetDeployedComponents()), "components."
	core.TriggerStart();
	core.Start(False)
        for component in core.GetDeployedComponents():
            for item in component.mStatus:
                print "item", item.mMessage
                self.fail()
        i = 0
        while core.IsTriggerActive():
            core.Step()
            for component in core.GetDeployedComponents():
                for item in component.mStatus:
                    print "item", item.mMessage
                    self.fail()
            image = core.mDataStructureImageColor.mImage
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

