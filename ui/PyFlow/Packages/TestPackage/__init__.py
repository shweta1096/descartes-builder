PACKAGE_NAME = 'TestPackage'

from collections import OrderedDict
from PyFlow.UI.UIInterfaces import IPackage

# Pins
from PyFlow.Packages.TestPackage.Pins.DemoPin import DemoPin
from PyFlow.Packages.TestPackage.Pins.DataPin import DataPin
from PyFlow.Packages.TestPackage.Pins.FunctionPin import FunctionPin

# Function based nodes
from PyFlow.Packages.TestPackage.FunctionLibraries.KedroLib import KedroBlock

# Class based nodes
from PyFlow.Packages.TestPackage.Nodes.DemoNode import DemoNode
from PyFlow.Packages.TestPackage.Nodes.CoderNode import CoderNode
from PyFlow.Packages.TestPackage.Nodes.ProcessorNode import ProcessorNode
from PyFlow.Packages.TestPackage.Nodes.TrainerNode import TrainerNode

# Tools
from PyFlow.Packages.TestPackage.Tools.DemoShelfTool import DemoShelfTool
from PyFlow.Packages.TestPackage.Tools.DemoDockTool import DemoDockTool

# Exporters
from PyFlow.Packages.TestPackage.Exporters.DemoExporter import DemoExporter

# Factories
from PyFlow.Packages.TestPackage.Factories.UIPinFactory import createUIPin
from PyFlow.Packages.TestPackage.Factories.UINodeFactory import createUINode
from PyFlow.Packages.TestPackage.Factories.PinInputWidgetFactory import getInputWidget
# Prefs widgets
from PyFlow.Packages.TestPackage.PrefsWidgets.DemoPrefs import DemoPrefs

_FOO_LIBS = {}
_NODES = {}
_PINS = {}
_TOOLS = OrderedDict()
_PREFS_WIDGETS = OrderedDict()
_EXPORTERS = OrderedDict()

_FOO_LIBS[KedroBlock.__name__] = KedroBlock(PACKAGE_NAME)

_PACKAGE_NODES = [DemoNode, CoderNode, ProcessorNode, TrainerNode]

for node in _PACKAGE_NODES:
	_NODES[node.__name__] = node

_PACKAGE_PINS = [DemoPin, DataPin, FunctionPin]

for pin in _PACKAGE_PINS:
	_PINS[pin.__name__] = pin

_TOOLS[DemoShelfTool.__name__] = DemoShelfTool
_TOOLS[DemoDockTool.__name__] = DemoDockTool

_EXPORTERS[DemoExporter.__name__] = DemoExporter

_PREFS_WIDGETS["Demo"] = DemoPrefs


class TestPackage(IPackage):
	def __init__(self):
		super(TestPackage, self).__init__()

	@staticmethod
	def GetExporters():
		return _EXPORTERS

	@staticmethod
	def GetFunctionLibraries():
		return _FOO_LIBS

	@staticmethod
	def GetNodeClasses():
		return _NODES

	@staticmethod
	def GetPinClasses():
		return _PINS

	@staticmethod
	def GetToolClasses():
		return _TOOLS

	@staticmethod
	def UIPinsFactory():
		return createUIPin

	@staticmethod
	def UINodesFactory():
		return createUINode

	@staticmethod
	def PinsInputWidgetFactory():
		return getInputWidget

	@staticmethod
	def PrefsWidgets():
		return _PREFS_WIDGETS

