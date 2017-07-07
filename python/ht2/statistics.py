counter200 = 0
counter300 = 0
counterOther = 0
lines = [line for line in open('input.txt')]
for it in lines:
    var = it.split(' ')[8]
    if var == "200":
        counter200 += 1
    elif "300" <= var <= "309":
        counter300 += 1
    else:
        counterOther += 1
print(counter200)
print(counter300)
print(counterOther)
print(counter300 + counter200 + counterOther)
