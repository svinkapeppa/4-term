import sys
import time
from functools import wraps


def profiler(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        wrapper.local_calls += 1
        wrapper.recursion_depth += 1
        value = func(*args, **kwargs)
        wrapper.recursion_depth -= 1
        if wrapper.recursion_depth == 0:
            wrapper.calls = wrapper.local_calls
            wrapper.local_calls = 0
            wrapper.last_time_taken = time.time() - start
        return value
    wrapper.local_calls = 0
    wrapper.recursion_depth = 0
    wrapper.last_time_taken = 0
    wrapper.calls = 0
    return wrapper


exec(sys.stdin.read())
