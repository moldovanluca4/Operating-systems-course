import pandas as pd

df = pd.read_csv('python-memprof.csv')
allocated_size = df['2']

print(df['2'].sum(skipna=True))

print('malloc:')
getting_malloc = df.loc[df['malloc'] == 'malloc', '2']
print(getting_malloc.min())
print(getting_malloc.max())
print(f"{getting_malloc.mean():.2f}")

getting_calloc = df.loc[df['malloc'] == 'calloc', '2']

print('calloc:')
print(getting_calloc.min())
print(getting_calloc.max())
print(f"{getting_calloc.mean():.2f}")

getting_realloc = df.loc[df['malloc'] == 'realloc', '2']

print('realloc:')
print(getting_realloc.min())
print(getting_realloc.max())
print(f"{getting_realloc.mean():.2f}")

getting_free = df.loc[df['malloc'] == 'free', '2']

print('free:')
print(getting_free.min(skipna=True))
print(getting_free.max(skipna=True))
print(f"{getting_free.mean(skipna=True):.2f}")



top_10_sizes = df['2'].value_counts().head(10)

print('top 10 sizes:')
print(top_10_sizes)

pointer_mallocs = df.loc[df['malloc'] == 'malloc', '0x557fb05672a0']

set_mallocs = set(pointer_mallocs)

pointer_callocs = df.loc[df['malloc'] == 'calloc', '0x557fb05672a0']

set_callocs = set(pointer_callocs)

intersection = set_callocs & set_mallocs


print(intersection)