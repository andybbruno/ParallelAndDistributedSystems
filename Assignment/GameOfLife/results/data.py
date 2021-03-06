import pandas as pd
import matplotlib.pyplot as plt

df0 = pd.read_csv("seq.csv")
df1 = pd.read_csv("par_active.csv")
df2 = pd.read_csv("par_passive.csv")
df3 = pd.read_csv("par_omp.csv")
df4 = pd.read_csv("par_ff.csv")


# for nw in [4, 8, 16, 32, 64, 128, 256]:
#     for i in [16, 32, 64, 128, 256]:
#         t_mean = df1[(df1.nw == nw) & (df1.rowcol == i)].mean()

seq = df0.groupby(['nw', 'rowcol']).mean().reset_index()
par1 = df1.groupby(['nw', 'rowcol']).mean().reset_index()
par2 = df2.groupby(['nw', 'rowcol']).mean().reset_index()
par3 = df3.groupby(['nw', 'rowcol']).mean().reset_index()
par4 = df4.groupby(['nw', 'rowcol']).mean().reset_index()


for nw in [1, 4, 16, 64, 256]:
    fig = plt.figure()
    tmp1 = par1[(par1.nw == nw)]
    tmp2 = par2[(par2.nw == nw)]
    tmp3 = par3[(par3.nw == nw)]
    tmp4 = par4[(par4.nw == nw)]
    a, = plt.plot(seq['rowcol'], seq['time'], 'o-k', label='Seq')
    b, = plt.plot(tmp1['rowcol'], tmp1['time'], '-*r', label='ActiveFarm')
    c, = plt.plot(tmp2['rowcol'], tmp2['time'], 's-.g', label='PassiveFarm')
    d, = plt.plot(tmp3['rowcol'], tmp3['time'], '^--y', label='ParallelForOMP')
    e, = plt.plot(tmp4['rowcol'], tmp4['time'], ':Dm', label='ParallelForFF')
    plt.yscale('log')
    plt.xscale('log')
    plt.ylabel('Time')
    plt.xlabel('Size')
    plt.title("NW: " + str(nw))
    plt.legend(handles=[a,b,c,d,e])
    plt.show()
    fig.savefig('time_size' + str(nw) + '.png')

for col in [256, 1024, 4096, 16384]:
    fig = plt.figure()
    tseq = seq[(seq.rowcol == col)]['time']
    tseq = tseq.iloc[0]
    tmp1 = par1[(par1.rowcol == col)]
    tmp2 = par2[(par2.rowcol == col)]
    tmp3 = par3[(par3.rowcol == col)]
    tmp4 = par4[(par4.rowcol == col)]


    plt.plot([0, 256], [0, 256],'k')
    a, = plt.plot(tmp1['nw'], tseq / tmp1['time'], '-*r', label='ActiveFarm')
    b, = plt.plot(tmp2['nw'], tseq / tmp2['time'], 's-.g', label='PassiveFarm')
    c, = plt.plot(tmp3['nw'], tseq / tmp3['time'], '^--y', label='ParallelForOMP')
    d, = plt.plot(tmp4['nw'], tseq / tmp4['time'], ':Dm', label='ParallelForFF')

    plt.yscale('log')
    plt.xscale('log')
    plt.ylabel('SpeedUp')
    plt.xlabel('NW')
    plt.title("SPEEDUP ::: Size: " + str(col))
    plt.legend(handles=[a,b,c,d])
    plt.show()
    fig.savefig('speedup' + str(col) + '.png')

for col in [256, 1024, 4096, 16384]:
    fig = plt.figure()
    
    tt1 = par1[(par1.rowcol == col) & (par1.nw == 1)]['time'].iloc[0]
    tt2 = par2[(par2.rowcol == col) & (par2.nw == 1)]['time'].iloc[0]
    tt3 = par3[(par3.rowcol == col) & (par3.nw == 1)]['time'].iloc[0]
    tt4 = par4[(par4.rowcol == col) & (par4.nw == 1)]['time'].iloc[0]

    tmp1 = par1[(par1.rowcol == col)]
    tmp2 = par2[(par2.rowcol == col)]
    tmp3 = par3[(par3.rowcol == col)]
    tmp4 = par4[(par4.rowcol == col)]

    plt.plot([0, 256], [0, 256],'k')
    a, = plt.plot(tmp1['nw'], tt1 / tmp1['time'], '-*r', label='ActiveFarm')
    b, = plt.plot(tmp2['nw'], tt2 / tmp2['time'], 's-.g', label='PassiveFarm')
    c, = plt.plot(tmp3['nw'], tt3 / tmp3['time'], '^--y', label='ParallelForOMP')
    d, = plt.plot(tmp4['nw'], tt4 / tmp4['time'], ':Dm', label='ParallelForFF')

    plt.yscale('log')
    plt.xscale('log')
    # plt.ylabel('SpeedUp')
    plt.xlabel('NW')
    plt.title("SCALABILITY ::: Size: " + str(col))
    plt.legend(handles=[a,b,c,d])
    plt.show()
    fig.savefig('scalability' + str(col) + '.png')

# res0 = res0[(res0.rowcol == 256)]
# res0.plot(x='rowcol', y='time')
# plt.show()


# res1 = res1[(res1.nw == 4)]
# res1.plot()
# plt.show()


# res1.plot()
# res2.plot()
# res3.plot()

# print("ciao")
