counter = {'OS X': 0, 'Ubuntu': 0, 'Windows': 0, 'Unknown': 0}
lines = [line for line in open('input.txt')]
for it in lines:
    if it.split('\"')[5].find("Macintosh") >= 0:
        counter['OS X'] += 1
    elif it.split('\"')[5].find("Ubuntu") >= 0:
        counter['Ubuntu'] += 1
    elif it.split('\"')[5].find("Windows") >= 0:
        counter['Windows'] += 1
    else:
        counter['Unknown'] += 1
for i in sorted(counter.items(), key=lambda x: x[1]):
    print '%s: %s' % (i[0], i[1])
