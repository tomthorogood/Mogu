import moguio
import moguio.FileImporter
import moguio.PythonObjectConverter
from collections import OrderedDict

def printcontainer(container):
    if isinstance(container,(dict,OrderedDict)):
        print("Printing Dict:\n")
        for entry in container:
            print ("%s:\t%s"% (entry, container[entry]))
    elif isinstance(container,(list,tuple)):
        print("Printing %s" % str(type(container)))
        for entry in container:
            if isinstance(entry,(dict,OrderedDict,list,tuple)):
                printcontainer(entry)
            else:
                print entry

results = moguio.FileImporter.import_file("perspective.mogu")
converter = moguio.PythonObjectConverter.PythonObjectConverter()

print "Length of results: %d" % len(results)

types = {}

for res in results:
    t = type(res)
    if t not in types:
        types[t] = 0
    types[t] += 1

for t in types:
    print "%s   :   %d" % (t,types[t])

for result in results:
    print"\n\n"
    od = result[0]
    for entry in od:
        print "%s   :   %s" %(entry,str(od[entry]))

conversions = []

for result in results:
    conversions.append(converter.convert(result))

print("\n\nCONVERTED RESULTS")
printcontainer(conversions)
