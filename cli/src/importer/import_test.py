import RootConsumer
import PythonObjectConverter
import RedisWriter
from WIDGET_BLOCK import WIDGET_BLOCK

f = open("syntax_test.mogu")
root_result = RootConsumer.RootConsumer.parse(f.read())
f.close()

writer = RedisWriter.RedisWriter()
converter = PythonObjectConverter.PythonObjectConverter()

redis_objects = []
for entry in root_result:
    redis_objects.extend(converter.convert(entry))

writer.write(redis_objects)
