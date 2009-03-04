import unittest

class SwigTestCase(unittest.TestCase):
    def testBackgroundSubtraction(self):
        import swistrack
        core = swistrack.SwisTrackCore("../../SwisTrackEnvironment")
        subtractor = swistrack.ComponentBackgroundSubtractionCheungKamath()
        
if __name__ == "__main__":
    unittest.main()

