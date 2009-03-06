import swistrack
class SwistrackError(Exception):
    def __init__(self, msg):
        Error.__init__(self, msg)
class Pipeline:
    def __init__(self, componentsDirectory, pipelineFile):
        self.core = swistrack.SwisTrackCore(componentsDirectory)
        cr = swistrack.ConfigurationReaderXML()
        if not cr.Open("backgroundSubtraction.swistrack"):
            raise SwistrackException("Coudln't read file: %s" % pipelineFile)

	cr.ReadComponents(self.core)
        for x in cr.mErrorList.mList:
            print "x", x.mMessage
        if len(cr.mErrorList.mList) != 0:
            raise SwistrackException("Errors in file: %s" % pipelineFile)
        self.core.TriggerStart();
	self.core.Start(False)
        if self.printErrors():
            raise SwistrackException("Errors in file: %s" % pipelineFile)

    def step(self):
        self.core.Step()
        if self.printErrors():
            raise SwistrackException("Errors in step.")
    

     
    def printErrors(self):
        sawErrors = False
        for component in self.core.GetDeployedComponents():
            for item in component.mStatus:
                print "item", item.mMessage
                sawErrors = True
        return sawErrors
