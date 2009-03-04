import unittest

class SwigTestCase(unittest.TestCase):
    def testImport(self):
        import swistrack
    
    def testBackgroundSubtraction(self):
        subtractor = swistrack.ComponentBackgroundSubtractionCheungKamath()
        
if __name__ == "__main__":
    unittest.main()

