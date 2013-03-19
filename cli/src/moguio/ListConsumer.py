from lex_base import LIST_DEFINITION
from lex_base import NEWLINES
from pyRex import Consumer

ListConsumer = Consumer.Consumer([
    LIST_DEFINITION,
    NEWLINES
])
