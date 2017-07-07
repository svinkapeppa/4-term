class MyException:
    pass
try:
    raise MyException()
except Exception as e:
    print "Catch it"