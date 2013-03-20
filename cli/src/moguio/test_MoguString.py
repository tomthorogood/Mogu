import unittest
from MoguString import MoguString

class MoguStringTest(unittest.TestCase):

    def setUp(self):
        self.mogustrA = MoguString('script', "set widget foo content to widget bar css")
        self.mogustrB = MoguString('script', "set own css to \"someClass anotherClass\"")
        self.mogustrC = MoguString('script', "set own content to ((100 + user dob year) - 2013)")

    def testA(self):
        """
        Tests a MoguString that deals only in objects and attributes
        """
        string = self.mogustrA
        assert (string.translate('integral')) == "22 59 foo 30 56 59 bar 27"
        print("\n%s\t->\t%s\n" % (string,string.integral))

    def testB(self):
        """
        Tests a MoguString that has an object, attribute, and string literal
        """

        string = self.mogustrB
        assert (string.translate('integral')) == '22 1 27 56 "someClass anotherClass"'
        print("\n%s\t->\t%s\n" % (string,string.integral))

    def testC(self):
        """
        Tests a MoguString that has an object, attribute, and 
        mathematical expression
        """

        string = self.mogustrC
        string.translate('integral')
        print("\n%s\t->\t%s\n" % (string,string.integral))
        assert(string.translate('integral')) == "22 1 30 56 2147483642 2147483642 100 2147483644 24 dob year 2147483643 2147483645 2013 2147483643"

if __name__ == "__main__":
    unittest.main()
