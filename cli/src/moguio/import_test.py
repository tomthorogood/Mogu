import RootConsumer
import PythonObjectConverter
import RedisWriter
import SharedData
import SymbolRegistry
import pyboro
pyboro.Lexer.VERBAL = True
from WIDGET_BLOCK import WIDGET_BLOCK
SharedData.ActiveFile = "syntax_text.mogu"
f = open("syntax_test.mogu")
root_result = RootConsumer.RootConsumer.parse(f.read())
f.close()

writer = RedisWriter.RedisWriter()
converter = PythonObjectConverter.PythonObjectConverter()

redis_objects = []
for entry in root_result:
    redis_objects.extend(converter.convert(entry))

writer.write(redis_objects)

print("WIDGETS:")
print SymbolRegistry.widgetRegistry
for symbol in SymbolRegistry.widgetRegistry:
    print SymbolRegistry.widgetRegistry[symbol]

print("DATA:")
print SymbolRegistry.dataRegistry
for symbol in SymbolRegistry.dataRegistry:
    print SymbolRegistry.dataRegistry[symbol]
