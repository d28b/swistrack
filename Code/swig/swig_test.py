import unittest
from os.path import abspath
from opencv.highgui import *
import swistrackPipeline
components = abspath("../../SwisTrackEnvironment/Components/")
class SwigTestCase(unittest.TestCase):
    def testPipeline(self):
        import swistrack
        pipeline = swistrackPipeline.Pipeline(components, 
                                              "backgroundSubtraction.swistrack")

        for i in range(0,10):
            pipeline.step()
        image = pipeline.core.mDataStructureImageBinary.mImage        
        cvSaveImage("test.jpg", image)        

        
        
if __name__ == "__main__":
    unittest.main()

