import unittest

class SwigTestCase(unittest.TestCase):
    def testBackgroundSubtraction(self):
        import swistrack
        core = swistrack.SwisTrackCore("../../SwisTrackEnvironment")
        subtractor = swistrack.ComponentBackgroundSubtractionCheungKamath(core)
        from opencv.highgui import cvLoadImage
        image = cvLoadImage("earth-horizon.jpg")
        ipImage = cvGetImage(image)
        core.mDataStructureInput.mImage = ipImage
        
        
if __name__ == "__main__":
    unittest.main()

