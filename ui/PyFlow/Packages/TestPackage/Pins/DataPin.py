from PyFlow.Core import PinBase
from PyFlow.Core.Common import *

class FakeTypeTQXEH(object):
    """docstring for FakeTypeTQXEH"""
    def __init__(self, value=None):
        super(FakeTypeTQXEH, self).__init__()
        self.value = value


class DataPin(PinBase):
    """doc string for DataPin"""
    def __init__(self, name, parent, direction, **kwargs):
        super(DataPin, self).__init__(name, parent, direction, **kwargs)
        self.setDefaultValue(False)

    @staticmethod
    def IsValuePin():
        return True

    @staticmethod
    def supportedDataTypes():
        return ('DataPin',)

    @staticmethod
    def pinDataTypeHint():
        return 'DataPin', False

    @staticmethod
    def color():
        return (200, 200, 50, 255)

    @staticmethod
    def internalDataStructure():
        return FakeTypeTQXEH

    @staticmethod
    def processData(data):
        return DataPin.internalDataStructure()(data)
