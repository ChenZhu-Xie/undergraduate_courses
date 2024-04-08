import functools
max2 = functools.partial(max, 10,8)
max2(5, 6, 7)