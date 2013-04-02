import pyboro 
from moguio import FileImporter
pyboro.Lexer.VERBAL = True
parseable = FileImporter.stage("syntax_test.mogu", True)

if __name__ == "__main__":
    FileImporter.import_file("syntax_test.mogu")
