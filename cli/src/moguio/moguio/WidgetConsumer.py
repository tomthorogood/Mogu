from pyboro import Consumer
from lex_base import *
from EVENT_BLOCK import EVENT_BLOCK
from CHILDREN_BLOCK import CHILDREN_BLOCK

WidgetConsumer = Consumer.Consumer([
    WIDGET_TYPE,
    WIDGET_CONTENT,
    WIDGET_SOURCE,
    WIDGET_LOCATION,
    WIDGET_TEMPLATE,
    WIDGET_STYLE,
    EVENT_BLOCK,
    CHILDREN_BLOCK,
    NEWLINES
])
