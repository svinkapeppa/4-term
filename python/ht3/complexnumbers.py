import sys


class ComplexNumber:
    def __init__(self, real=0, imaginary=0):
        self.real = real
        self.imaginary = imaginary

    def __add__(self, other):
        return ComplexNumber(self.real + other.real,
                             self.imaginary + other.imaginary)

    def __sub__(self, other):
        return ComplexNumber(self.real - other.real,
                             self.imaginary - other.imaginary)

    def __mul__(self, other):
        return ComplexNumber(self.real * other.real -
                             self.imaginary * other.imaginary,
                             self.real * other.imaginary +
                             self.imaginary * other.real)

    def __div__(self, other):
        return ComplexNumber(
            (self.real * other.real + self.imaginary * other.imaginary) /
            (other.real ** 2 + other.imaginary ** 2),
            (self.imaginary * other.real - self.real * other.imaginary) /
            (other.real ** 2 + other.imaginary ** 2))

    def __str__(self):
        if self.imaginary == 0:
            return '%.2f' % self.real
        elif self.real == 0:
            return '%.2fi' % self.imaginary
        else:
            if self.imaginary > 0:
                return '%.2f + %.2fi' % (self.real, self.imaginary)
            else:
                return '%.2f - %.2fi' % (self.real, self.imaginary * (-1.0))


if __name__ == "__main__":
    for line in sys.stdin.readlines():
        print eval(line.strip())
