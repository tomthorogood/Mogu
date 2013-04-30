# ONLY STATIC DATA WILL BE STORED IN THIS FASHION!
# It will be loaded into the application as static objects,
# making it quicker to access when loading static data.
# Therefore 'data','meta','user', and 'group' prefixes will 
# still be stored as standard Redis objects.


from moguio.MoguString import MoguString

# SETUP -- Only change 'HUMAN_REGISTERS' if needed.
HUMAN_REGISTERS = [
    "LITERALS"      , 
    "IDENTIFIERS"   , 
]

BINARY_REGISTERS = [
    "COMMANDS"      , 
    "WIDGETS"       ,
    "TEMPLATES"     ,
    "POLICIES"      ,
    "VALIDATORS"
]

REGISTERS = {} 

REG_NUM = 0 # For more readable code
REG_INT = 1 # when invoking registers
BIN_NUM = 2 
REG_BIN = 3

for index,register in enumerate(HUMAN_REGISTERS):
    rnum = 99-index
    REGISTERS[register] =  (rnum,[])

for index,register in enumerate(BINARY_REGISTERS):
    rnum = 99-len(HUMAN_REGISTERS)-index
    REGISTERS[register] = (rnum, {}, "bin.%d" % rnum, {})

def is_int(token):
    try:
        int(token)
        return True
    except:
        return False


def binary_map(integer_list):
    # Make sure that every entry is an integral string
    integer_list = [str(int(i)) for i in integer_list]
    
    #integer_list = ['123', '45', '6']
    #expected result = '100101'

    integer = "".join(integer_list)
    binary = 1
    length = len(integer)

    binary <<= (length-1)
    offset  = len(integer_list[0])

    for i in integer_list[1:]:
        shift = length - 1 - offset
        b = (1 << shift)
        binary |= b
        offset += len(i)

    return "{0:b}".format(binary)

def encode_human(register, symbol):
    reg = REGISTERS[register][1]

    if symbol in reg:
        return reg.index(symbol)

    index = len(reg)
    reg.append(symbol)
    return index

def encode_literal(symbol):
    return encode_human("LITERALS", symbol)

def encode_identifier(symbol):
    return encode_human("IDENTIFIERS", symbol)

def encode_dict(prefix,dictionary):
    """
        All key:value pairs in the dict must already be translated
        into Mogu syntax in order for this to work properly.

        To determine how to store each 'value', we'll use the following
        algorithm:
            1) Assign string literals a representation, and replace them
            2) Assign identifiers a representation, and replace them
            3) split the string by a space delimiter, thus having a list
               of integers.
            4) Combine into a single integer string, and create binary map
            5) Store the integer and its binary map.
    """
    
    encoding = ""
    binary = ""

    for entry in dictionary:
        value = MoguString("integral", dictionary[entry])
        delimited = value.separate_string_literals()
        # Replace literal values with assigned symbols
        for index,literal in enumerate(value.string_literals):
            value.string_literals[index] = \
                    str(REGISTERS['LITERALS'][REG_NUM]) + \
                    str(encode_literal(literal))
        if (value.string_literals):
            value = delimited % tuple(value.string_literals)

        tokens = value.split(' ')

        # Any remaining non-integers in the string must necessarily
        # be identifiers. Replace them.
        for index,token in enumerate(tokens):
            if not is_int(token):
                token_id = encode_identifier(token)
                tokens[index] = \
                    str(REGISTERS['IDENTIFIERS'][REG_NUM]) + \
                    str(token_id)
        binary += binary_map([entry])
        binary += binary_map(tokens)
        int_string = "".join(tokens)
        encoding += str(entry) + int_string

    return {"int" : hex(int(encoding))[:-1], "bin" : binary}

if __name__ == "__main__":
    from moguio.MoguString import MoguString
    testdict = {
            "type"  :   "text",
            "text"  :   "widget foo content",
            "css"   :   "\"some css goes here\""
            }

    converteddict = {}
    for entry in testdict:
        mentry = MoguString("script",entry)
        mvalue = MoguString("script",testdict[entry])
        mentry.translate('integral')
        mvalue.translate('integral')

        converteddict[mentry.integral] = mvalue.integral

    result = encode_dict('WIDGETS', converteddict)
    print(result)
    print(REGISTERS['LITERALS'],REGISTERS['IDENTIFIERS'])
