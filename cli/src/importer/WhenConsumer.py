from pyRex import Consumer
from MOGU_CMD import MOGU_CMD
from lex_base import NEWLINES

WhenConsumer = Consumer.Consumer([
    MOGU_CMD,
    NEWLINES
])
