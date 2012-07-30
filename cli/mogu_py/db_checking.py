from coloring import *

class AmbiguousDirectiveError(Exception):
    def __init__(self, key, value):
        self.key = key
        self.value = value
    def __str__(self):
        value = ("Unable to successfully parse %s directive." %key)
        value += ("\nMust be of a specific type, which was not clearly defined in %s" % value)
        return value

class MissingParameterError(Exception):
    def __init__(self, widget_name, missing_parameter):
        self.value = ("Widget %s is missing parameter %s" % (widget_name, missing_parameter))

    def __str__(self):
        return self.value

required_widget_parameters = {
        "container" :           (None,),
        "stacked"   :           (None,),
        "image" :               ("content","source"),
        "image_link" :          ("content","source","location"),
        "link" :                ("content","location"),
        "text"  :               ("content",)
        }

required_event_parameters = {
        "102"   :               ("action","message","nextAction","degradation"),
        "103"   :               ("action","message","nextAction","degradation","trigger"),
        "106"   :               ("action","message","degradation"),
        "107"   :               ("action","message","degradation","trigger"),
        "112"   :               ("action","message","listeners"),
        "113"   :               ("action","message","listeners","trigger"),
        "118"   :               ("action","message","listeners","nextAction","degradation"),
        "119"   :               ("action","message","listeners","nextAction","degradation","trigger"),
        "122"   :               ("action","message","listeners","degradation"),
        "123"   :               ("action","message","listeners","degradation","trigger")
        }


class Wrappers:
    CURLY_BRACES = ('{','}')
    SQUARE_BRACES = ('[',']')
    PARENTHESES = ('(',')')
    ASTERISKS = ('*','*')
    AT_SYMBOLS = ('@','@')
    ANGLE_BRACKETS = ('<','>')
    CARATS = ('^','^')
    TILDES = ('~','~')
    BACKTICKS = ('`','`')
    NONE = None

    lookup = {
            '{' :   CURLY_BRACES,
            '}' :   CURLY_BRACES,
            '[' :   SQUARE_BRACES,
            ']' :   SQUARE_BRACES,
            '(' :   PARENTHESES,
            ')' :   PARENTHESES,
            '*' :   ASTERISKS,
            '@' :   AT_SYMBOLS,
            '<' :   ANGLE_BRACKETS,
            '>' :   ANGLE_BRACKETS,
            '^' :   CARATS,
            '~' :   TILDES,
            '`' :   BACKTICKS
            }

class Type_Assertions:
    TYPE    =       (Wrappers.CURLY_BRACES,)
    CLASS   =       (Wrappers.NONE,Wrappers.ANGLE_BRACKETS)
    CHILDREN =      (Wrappers.ANGLE_BRACKETS, Wrappers.AT_SYMBOLS)
    CONTENT =       (
            Wrappers.NONE,          # Standard text content
            Wrappers.SQUARE_BRACES, # Text content a field in this hash (rare but possible)
            Wrappers.BACKTICKS,     # Content stored in external file
            Wrappers.AT_SYMBOLS,    # Content stored in dynamic node
            Wrappers.ANGLE_BRACKETS # Content stored in static node
            )
    SOURCE =        CONTENT
    LOCATION =      CONTENT
    SET_INDEX =     (Wrappers.CARATS,)
    SET_STYLE =     CONTENT
    SET_CONTENT =   CONTENT
    ACTION =        (Wrappers.CURLY_BRACES,)
    DEGRADATION =   (Wrappers.CARATS,)
    SIGNAL =        (Wrappers.CARATS,Wrappers.CURLY_BRACES)
    TRIGGER =       (Wrappers.CURLY_BRACES,)
    MESSAGE =       CONTENT
    LISTENERS =     (Wrappers.CURLY_BRACES,)
    EVENTS =        (Wrappers.NONE,Wrappers.ANGLE_BRACKETS)
    BLOCK  =        (Wrappers.CURLY_BRACES,)
    NAME    =       (Wrappers.NONE,)

key_checklist = {
        "type"  :       Type_Assertions.TYPE,
        "class" :       Type_Assertions.CLASS,
        "chidlren" :    Type_Assertions.CHILDREN,
        "content"   :   Type_Assertions.CONTENT,
        "source"    :   Type_Assertions.SOURCE,
        "location"  :   Type_Assertions.LOCATION,
        "set_index" :   Type_Assertions.SET_INDEX,
        "set_content" : Type_Assertions.SET_CONTENT,
        "action"    :   Type_Assertions.ACTION,
        "nextAction" :  Type_Assertions.ACTION,
        "degradation" : Type_Assertions.DEGRADATION,
        "signal"    :   Type_Assertions.SIGNAL,
        "trigger"   :   Type_Assertions.TRIGGER,
        "message"   :   Type_Assertions.MESSAGE,
        "listeners" :   Type_Assertions.LISTENERS,
        "events"    :   Type_Assertions.EVENTS,
        "block"     :   Type_Assertions.BLOCK,
        "name"      :   Type_Assertions.NAME
        }

def is_wrapped(value,start,finish):

    # If the value does not start with the required value
    if value[0] is not start:
        # But does end with the required value
        if value[len(value)-1] == finish:
            return -1
        return 0
    # If it starts with the required character but does not end with it
    if value[len(value)-1] is not finish:
        return -2
    # Otherwise
    return 1

def wrap(value,start,finish):
    return "%s%s%s" % (start,value,finish)

def assert_wrap(value,char_set):

    wrap_status = is_wrapped(value,char_set[0],char_set[1])
    
    if wrap_status is 1:
        return value
    elif wrap_status is -1:
        return wrap(value[0:-1],char_set[0],char_set[1])
    elif wrap_status is -2:
        return wrap(value[1:],char_set[0],char_set[1])
    else:
        return wrap(value,char_set[0],char_set[1])

def check(key,value,test=False):
    try:
        checklist = key_checklist[key]
        assert_as_type = None
        first_char = value[0]
        last_char = value[-1]
        for wrapper_type in checklist:
            if wrapper_type is not Wrappers.NONE:
                if first_char == wrapper_type[0] or last_char == wrapper_type[1]:
                    assert_as_type = wrapper_type
                    break
        if not assert_as_type:
            if Wrappers.NONE in checklist:
                return value
            elif len(checklist) is 1:
                assert_as_type = checklist[0]
            else:
                raise AmbiguousDirectiveError(key,value)

        if assert_as_type:
            return assert_wrap(value,assert_as_type)
    except AmbiguousDirectiveError:
        if test:
            value = "ERROR"
        else:
            raise AmbiguousDirectiveError(key,value)

    return value

def check_widget_values(widget_name,args):
    for key in args:
        args[key] = check(key,args[key])
    
    unwrapped_type = args['type'][1:-1]
    required_params = required_widget_parameters[unwrapped_type]
    if required_params[0] is not None:
        results = [False for i in range(len(required_params))]
        i = 0
        for param in required_params:
            results[i] = param in args
            i+=1
        if not all(results):
            missing_param = results.index(False)
            missing_param = required_params[missing_param]
            raise MissingParameterError(widget_name,missing_param)
    return args

def check_event_values(event_name, args):
    for key in args:
        args[key] = check(key,args[key])
    unwrapped_signal = args['signal'][1:-1]
    required_params = required_event_parameters[unwrapped_signal]
    results = [False for i in range(len(required_params))]
    i = 0
    for param in required_params:
        results[i] = param in args
        i+=1
    if not all(results):
        missing_param = results.index(False)
        missing_param = required_params[missing_param]
        raise MissingParameterError(event_name,missing_param)
    return args

if __name__ == "__main__":
    test_set = ( 
            ("type",    "foo",  "{foo}"),
            ("type",    "{foo", "{foo}"),
            ("type",    "foo}", "{foo}"),
            ("type",    "{foo}","{foo}"),
            ("class",   "foo",  "foo"),
            ("class",   "<foo", "<foo>"),
            ("class",   "foo>", "<foo>"),
            ("signal",  "89^",  "^89^"),
            ("signal",  "^89",  "^89^"),
            ("signal",  "foo}", "{foo}"),
            ("signal",  "{foo", "{foo}"),
            ("signal",  "{foo}","{foo}"),
            ("signal",  "foo", "ERROR"),
            ("events",  "on", "on"),
            ("message", "generic message", "generic message"),
            ("action", "{set_index}","{set_index}")

            )
    test_widget = {
            "type"  :   "{link}",
            "location" : "foo",
            "content" : "bar"
            }
    test_event = {
            "signal"    :   "^119^",
            "action"    :   "{bubble}",
            "message"   :   "^1^",
            "nextAction":   "{set_index}",
            "degradation":  "^2^",
            "trigger"   :   "{clicked}",
            "listeners" :   "{siblings}"
            }
    for test in test_set:
        test_output = check(test[0],test[1],test=True)
        result = test[2] == test_output
        if (result):
            result_output = tint("PASS", TermColors.OK)
        else:
            result_output = tint("FAIL", TermColors.FAIL)

        print("%s : %s ==> %s \t| %s" % (test[0],test[1],test_output,result_output))

    check_widget_values("test.widget", test_widget)
    check_event_values("test.event", test_event)
