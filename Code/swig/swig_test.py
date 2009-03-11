import unittest
from os.path import abspath
from opencv.highgui import *
from opencv import *


components = abspath("../../SwisTrackEnvironment/Components/")
class SwigTestCase(unittest.TestCase):
    def testPipeline(self):
        import swistrackPipeline
        import swistrack
        pipeline = swistrackPipeline.Pipeline(components, 
                                              "backgroundSubtraction.swistrack")

        for i in range(0,10):
            pipeline.step()
        image = pipeline.core.mDataStructureImageBinary.mImage        
        cvSaveImage("test.jpg", image)
        mat = swistrack.Utility.IpImageToCvMat(image)
        i1 = cvCloneImage(mat)
        
        
if __name__ == "__main__":
    unittest.main()

