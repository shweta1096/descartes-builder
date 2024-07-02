from PyFlow.Core.Common import *
from PyFlow.Core import FunctionLibraryBase
from PyFlow.Core import IMPLEMENT_NODE


class KedroBlock(FunctionLibraryBase):
    '''doc string for KedroLib'''

    def __init__(self, packageName):
        super(KedroBlock, self).__init__(packageName)

    @staticmethod
    @IMPLEMENT_NODE(returns=None, nodeType=NodeTypes.Callable, meta={NodeMeta.CATEGORY: 'KedroLib', NodeMeta.KEYWORDS: []})
    # Return a random integer N such that a <= N <= b
    def greet(word=('StringPin', "Greet!")):
        """Docstrings are in **rst** format!"""
        print(word)
