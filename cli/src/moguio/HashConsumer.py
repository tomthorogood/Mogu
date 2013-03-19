from lex_base import HASH_DEFINITION
from lex_base import NEWLINES
from pyRex import Consumer

HashConsumer = Consumer.Consumer([
    HASH_DEFINITION,
    NEWLINES
])
