import unittest

class SwigTestCase(unittest.TestCase):
    def testBackgroundSubtraction(self):
        import swistrack
        core = swistrack.SwisTrackCore("../../SwisTrackEnvironment")
        subtractor = swistrack.ComponentBackgroundSubtractionCheungKamath(core)
        
if __name__ == "__main__":
    unittest.main()

