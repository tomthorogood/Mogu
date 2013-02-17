from pyRex import Consumer
from lex_base import NEWLINES

import data_blocks as d

DataConsumer = Consumer.Consumer([
    d.HASH_BLOCK,
    d.LIST_BLOCK,
    d.VALUE_DEFINITION,
    NEWLINES
])
