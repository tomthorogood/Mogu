from lex import *
from pyRex import Consumer

RootConsumer = Consumer.Consumer([
    WIDGET_BLOCK,
    TEMPLATE_BLOCK,
    POLICY_BLOCK,
    DATA_BLOCK,
    VALIDATOR_BLOCK,
    NEWLINES,
])

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

if __name__ == "__main__":
    results = None
    pyRex.Lexer.VERBAL = True
    with open("syntax_test.mogu") as f:
        results = RootConsumer.parse(f.read())

    for result in results:
        for e in result[0]:
            print e,result[0][e]
        if result[1] is WIDGET_BLOCK:
            wresult = WidgetConsumer.parse(result[0]["widget_def"])
            for w in wresult:
                print w[0]
