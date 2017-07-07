a = [4.0]
k = -1
for i in range(1, 10):
    a.append(a[i - 1] + k * 4 / float(2 * i + 1))
    k *= -1
print a[9]
