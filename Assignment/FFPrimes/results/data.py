import pandas as pd
import matplotlib.pyplot as plt

df_seq = pd.read_csv("seq.csv")
tseq = df_seq.groupby(['nw']).mean().reset_index()['time'].iloc[0]

df_ofarm = pd.read_csv("ofarm.csv")
df_ofarm = df_ofarm.groupby(['nw']).mean().reset_index()

df_set = pd.read_csv("set.csv")
df_set = df_set.groupby(['nw']).mean().reset_index()

df_pfor = pd.read_csv("pfor.csv")


df_pfor = df_pfor.groupby(['nw', 'chunk']).mean().reset_index()
df_pfor1 = df_pfor[(df_pfor.chunk == -10)]
df_pfor2 = df_pfor[(df_pfor.chunk == 0)]
df_pfor3 = df_pfor[(df_pfor.chunk == 10)]

fig = plt.figure()
# a, = plt.plot(df_seq['time'], [1,1,1,1,1], 'o-k', label='Seq')
b, = plt.plot(df_ofarm['nw'], df_ofarm['time'], '-*r', label='OFarm')
c, = plt.plot(df_set['nw'], df_set['time'], 's-.g', label='Farm+Set')
d, = plt.plot(df_pfor1['nw'], df_pfor1['time'],'D:c',label='ParallelFor(-10)')
e, = plt.plot(df_pfor2['nw'], df_pfor2['time'],'^:m',label='ParallelFor(0)')
f, = plt.plot(df_pfor3['nw'], df_pfor3['time'],'o:b',label='ParallelFor(10)')
plt.legend(handles=[b, c, d, e, f])
plt.yscale('log')
plt.xscale('log')
plt.ylabel('Time')
plt.xlabel('NW')
plt.title("Time")
plt.show()


fig = plt.figure()
a, = plt.plot([0, 256], [0, 256], 'o-k', label='Seq')
b, = plt.plot(df_ofarm['nw'], df_ofarm['time'].div(tseq), '-*r', label='OFarm')
c, = plt.plot(df_set['nw'], df_set['time'].div(tseq), 's-.g', label='Farm+Set')
d, = plt.plot(df_pfor1['nw'], df_pfor1['time'].div(tseq),'D:c',label='ParallelFor(-10)')
e, = plt.plot(df_pfor2['nw'], df_pfor2['time'].div(tseq),'^:m',label='ParallelFor(0)')
f, = plt.plot(df_pfor3['nw'], df_pfor3['time'].div(tseq),'o:b',label='ParallelFor(10)')
plt.legend(handles=[b, c, d, e, f])
plt.yscale('log')
plt.xscale('log')
plt.ylabel('SpeedUp')
plt.xlabel('NW')
plt.title("SpeedUp")
plt.show()
plt.show()
