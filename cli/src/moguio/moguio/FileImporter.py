#!/usr/bin/env python
from moguio.MoguString import MoguString
from Lex import RootConsumer
import SharedData
import os
import Preprocessor
from Loggable import Loggable
from Loggable import LogMessage

def nonempty(string):
    return len(string.strip()) > 0 

class FileImporter(Loggable):
    def __init__(self, filename, verbose=0):
        super(FileImporter,self).__init__(verbose)
        SharedData.ActiveFile = filename
        self.filename = filename
        self.text = ""
    
    def stage_input(self):
        with open(self.filename,"r") as f:
            text = f.read()
        pre = Preprocessor.Preprocessor(self.verbose)
        path = os.path.dirname(self.filename)
        text = pre.processInput(text, path)

        if not isinstance(text, list):
            text = text.split('\n')
        text = [line.strip() for line in text]
        text = filter(nonempty, text)
        for index, line in enumerate(text):
            ms = MoguString("script", line)
            text[index] = ms
            text[index].translate('integral')
        text = [line.integral for line in text]
        self.text = "\n".join(text)

    def get_results(self):
        self.log(LogMessage("Importing %s..." % self.filename, 5), self.OUT)
        self.stage_input()
        self.results = RootConsumer.parse(self.text)
        return self.results
