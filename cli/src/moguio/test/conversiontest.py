from moguio.MoguString import MoguString
f = open("syntax_test.mogu")
mogustring = MoguString("script", f.read())
f.close()
print(mogustring.translate("integral"))
