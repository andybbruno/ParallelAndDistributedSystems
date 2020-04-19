import pandas as pd
import matplotlib.pyplot as plt

df_seq = pd.read_csv("seq.csv")

df_ofarm = pd.read_csv("ofarm.csv")
df_ofarm = df_ofarm.groupby(['nw']).mean().reset_index()

df_set = pd.read_csv("set.csv")
df_set = df_set.groupby(['nw']).mean().reset_index()

df_pfor = pd.read_csv("pfor.csv")
df_pfor = df_pfor.groupby(['nw','chunk']).mean().reset_index()


fig = plt.figure()
a, = plt.plot(df_seq['time'], 1, 'o-k', label='Seq')
b, = plt.plot(df_ofarm['time'], df_ofarm['nw'], '-*r', label='OFarm')
c, = plt.plot(df_set['time'], df_set['nw'], 's-.g', label='Farm+Set')
d, = plt.plot(df_pfor['time'], df_pfor['nw'], '^--y', label='ParallelFor')
plt.legend(handles=[a,b,c,d,e])
plt.show()