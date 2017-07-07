import sys


class Stack:
    def __init__(self, instance):
        self.store = []
        for i in instance:
            self.store.append(i)

    def pop(self):
        return self.store.pop()

    def top(self):
        x = self.store.pop()
        self.store.append(x)
        return x

    def push(self, x):
        self.store.append(x)

    def __len__(self):
        return len(self.store)

    def __str__(self):
        x = ''
        for i in self.store:
            x = x + str(i) + ' '
        return x


exec(sys.stdin.read())
