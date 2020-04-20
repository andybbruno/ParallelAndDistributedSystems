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
fig.savefig('time.png')


fig = plt.figure()
a, = plt.plot([0, 256], [0, 256], '-k')
b, = plt.plot(df_ofarm['nw'], tseq / df_ofarm['time'], '-*r', label='OFarm')
c, = plt.plot(df_set['nw'], tseq / df_set['time'], 's-.g', label='Farm+Set')
d, = plt.plot(df_pfor1['nw'], tseq / df_pfor1['time'],'D:c',label='ParallelFor(-10)')
e, = plt.plot(df_pfor2['nw'], tseq / df_pfor2['time'],'^:m',label='ParallelFor(0)')
f, = plt.plot(df_pfor3['nw'], tseq / df_pfor3['time'],'o:b',label='ParallelFor(10)')
plt.legend(handles=[b, c, d, e, f])
plt.yscale('log')
plt.xscale('log')
plt.ylabel('SpeedUp')
plt.xlabel('NW')
plt.title("SpeedUp")
plt.show()
fig.savefig('speedup.png')



t1ofarm = df_ofarm[(df_ofarm.nw == 1)]['time'].iloc[0]
t1set = df_set[(df_set.nw == 1)]['time'].iloc[0]
t1pfor1 = df_pfor1[(df_pfor1.nw == 1)]['time'].iloc[0]
t1pfor2 = df_pfor2[(df_pfor2.nw == 1)]['time'].iloc[0]
t1pfor3 = df_pfor3[(df_pfor3.nw == 1)]['time'].iloc[0]

fig = plt.figure()
a, = plt.plot([0, 256], [0, 256], '-k')
b, = plt.plot(df_ofarm['nw'], t1ofarm / df_ofarm['time'], '-*r', label='OFarm')
c, = plt.plot(df_set['nw'], t1set / df_set['time'], 's-.g', label='Farm+Set')
d, = plt.plot(df_pfor1['nw'], t1pfor1 / df_pfor1['time'],'D:c',label='ParallelFor(-10)')
e, = plt.plot(df_pfor2['nw'], t1pfor2 / df_pfor2['time'],'^:m',label='ParallelFor(0)')
f, = plt.plot(df_pfor3['nw'], t1pfor3 / df_pfor3['time'],'o:b',label='ParallelFor(10)')
plt.legend(handles=[b, c, d, e, f])
plt.yscale('log')
plt.xscale('log')
plt.ylabel('Scalability')
plt.xlabel('NW')
plt.title("Scalability")
plt.show()
fig.savefig('scalability.png')
