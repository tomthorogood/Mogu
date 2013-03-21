import unittest
from MoguString import MoguString
import syntax
class MoguStringTest(unittest.TestCase):

    def setUp(self):
        ambiguous = ("css", "text")
        prefs = {}

        for word in ambiguous:
            prefs[syntax.as_integer(word)] = word

        self.mogustrA = MoguString('script', "set widget foo content to widget bar css", prefs)
        self.mogustrB = MoguString('script', "set own css to \"someClass anotherClass\"", prefs)
        self.mogustrC = MoguString('script', "set own content to ((100 + user dob year) - 2013)", prefs)

    def testStatic(self):
        assert isinstance(MoguString.ReverseLookup,dict)
    
    def testA(self):
        """
        Tests a MoguString that deals only in objects and attributes
        """
        string = self.mogustrA
        assert (string.translate('integral')) == "22 59 foo 30 56 59 bar 27"
#        print("\n%s\t->\t%s\n" % (string,string.integral))
        string.active('integral')
        script = string.translate('script')
#        print(string.translate('script'))
        assert script == "set widget foo text to widget bar css"

    def testB(self):
        """
        Tests a MoguString that has an object, attribute, and string literal
        """

        string = self.mogustrB
        assert (string.translate('integral')) == '22 1 27 56 "someClass anotherClass"'
#        print("\n%s\t->\t%s\n" % (string,string.integral))
        string.active('integral')
        script = string.translate('script')
        assert script == "set self css to \"someClass anotherClass\""
#        print(string.translate('script'))

    def testC(self):
        """
        Tests a MoguString that has an object, attribute, and 
        mathematical expression
        """

        string = self.mogustrC
#        print("\n%s\t->\t%s\n" % (string,string.integral))
        assert(string.translate('integral')) == "22 1 30 56 2147483642 2147483642 100 2147483644 24 dob year 2147483643 2147483645 2013 2147483643"
        string.active('integral')
        script = string.translate('script')
#        print(script)
        assert script == "set self text to ( ( 100 + user dob year ) - 2013 )"

if __name__ == "__main__":
    unittest.main()
