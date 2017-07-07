for i in range(1, 101):
    print i,
print
for i in range(1, 101):
    if i % 15 == 0:
        print 'BazQux',
    elif i % 5 == 0:
        print 'Qux',
    elif i % 3 == 0:
        print 'Baz',
    else:
        print i,
