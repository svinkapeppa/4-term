import sys
from functools import wraps


def takes(*types):
    def takes_wrapper(func):
        @wraps(func)
        def wrapper(*args):
            for p, type_ in zip(args, types):
                if type(p) is not type_:
                    raise TypeError
            val = func(*args)
            return val
        return wrapper
    return takes_wrapper


exec(sys.stdin.read())
