from pyboro import Consumer
from WHEN_BLOCK import WHEN_BLOCK
from lex_base import NEWLINES

EventConsumer = Consumer.Consumer([
    WHEN_BLOCK,
    NEWLINES
])
