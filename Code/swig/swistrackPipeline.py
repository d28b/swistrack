import swistrack
from os.path import abspath
class SwistrackError(Exception):
    def __init__(self, msg):
        Exception.__init__(self, msg)
class Pipeline:
    def __init__(self, componentsDirectory, pipelineFile):
        self.pipelineFile = abspath(pipelineFile)
        self.core = swistrack.SwisTrackCore(abspath(componentsDirectory))
        cr = swistrack.ConfigurationReaderXML()
        if not cr.Open(self.pipelineFile):
            raise SwistrackError("Coudln't read file: %s" % self.pipelineFile)

	cr.ReadComponents(self.core)
        for x in cr.mErrorList.mList:
            print "x", x.mMessage
        if len(cr.mErrorList.mList) != 0:
            raise SwistrackError("Errors in file: %s" % self.pipelineFile)
        self.core.TriggerStart();
	self.core.Start(False)
        if self.printErrors():
            raise SwistrackError("Errors in file: %s" % self.pipelineFile)

    def step(self):
        self.core.Step()
        if self.printErrors():
            raise SwistrackError("Errors in step.")
    

     
    def printErrors(self):
        sawErrors = False
        for component in self.core.GetDeployedComponents():
            for item in component.mStatus:
                print "item", item.mMessage
                sawErrors = True
        return sawErrors
