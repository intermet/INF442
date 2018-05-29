import pandas as pd
import sys


df1 = pd.read_csv(sys.argv[1], sep=" ", header=None)[:10000]
df1.columns = ["x", "y"]

df2 = df1.copy()
df2.columns = ["y", "z"]

df = df1.merge(df2)
print(df.shape)
