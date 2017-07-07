counter = 0
lines = [line for line in open('input.txt')]
for it in lines:
    if it.split('\"')[5].find("Go-http-client/1.1") >= 0:
        counter += 1
print(counter)
