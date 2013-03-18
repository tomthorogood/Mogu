from lex_base import *
from pyRex import Consumer
# Base consumers are made up of ParseMaps that do not
# refer to other consumers.


PolicyConsumer = Consumer.Consumer([
    POLICY_MODE,
    POLICY_DATA,
    NEWLINES
])

ValidatorConsumer = Consumer.Consumer([
    VALIDATOR_TYPE,
    VALIDATOR_TEST,
    NEWLINES

])
