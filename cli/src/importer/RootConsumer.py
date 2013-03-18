from pyRex import Consumer
from WIDGET_BLOCK import WIDGET_BLOCK
from WIDGET_BLOCK import TEMPLATE_BLOCK
from POLICY_BLOCK import POLICY_BLOCK
from DATA_BLOCK import DATA_BLOCK
from VALIDATOR_BLOCK import VALIDATOR_BLOCK
from lex_base import NEWLINES

RootConsumer = Consumer.Consumer([
    WIDGET_BLOCK,
    TEMPLATE_BLOCK,
    POLICY_BLOCK,
    DATA_BLOCK,
    VALIDATOR_BLOCK,
    NEWLINES,
    ], help="Basic Mogu syntax in the form of object blocks. See http://www.github.com/tomthorogood/gomogu for more assistance.")
