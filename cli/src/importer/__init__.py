import PathImporter
import FileImporter
import SymbolRegistry
import PythonObjectConverter
import RedisWriter

def mogu_import(path, write=True):
    if path.endswith(".mogu"):
        results = FileImporter.import_file(path)
    else:
        results = PathImporter.import_path(path)
    converter = PythonObjectConverter.PythonObjectConverter()
    redis_objects = []
    for result in results:
        redis_objects.extend(converter.convert(result))

    if write:
        writer = RedisWriter.RedisWriter()
        writer.write(redis_objects)
