import sys
from functools import wraps


sys.setrecursionlimit(200000)


def memoize(func):
    cache = {}

    @wraps(func)
    def wrapper(*args, **kwargs):
        parameters = args + tuple(kwargs.items())
        if parameters in cache:
            return cache[parameters]
        else:
            x = func(*args, **kwargs)
            cache[parameters] = x
            return x
    return wrapper


exec(sys.stdin.read())
