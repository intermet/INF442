import pandas as pd
import sys


def check_simple_join():
    df1 = pd.read_csv(sys.argv[2], sep=" ", header=None)
    df1.columns = ["x", "y"]

    df2 = df1.copy()
    df2.columns = ["y", "z"]

    df = df1.merge(df2)
    print(df.shape)

def check_triangle_join():
    df1 = pd.read_csv(sys.argv[2], sep=" ", header=None)
    df1.columns = ["x", "y"]

    df2 = df1.copy()
    df2.columns = ["y", "z"]

    df3 = df1.copy()
    df3.columns = ["z", "x"]

    df = df1.merge(df2)
    df = df.merge(df3)
    print(df.shape)

if __name__ == "__main__":
    if sys.argv[1] == "simple":
        check_simple_join()
    elif sys.argv[1] == "triangle":
        check_triangle_join()
