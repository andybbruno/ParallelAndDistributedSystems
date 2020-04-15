import pandas as pd
import matplotlib.pyplot as plt

df0 = pd.read_csv("seq.csv")
df1 = pd.read_csv("par_active.csv")
df2 = pd.read_csv("par_passive.csv")
df3 = pd.read_csv("par_omp.csv")


# for nw in [4, 8, 16, 32, 64, 128, 256]:
#     for i in [16, 32, 64, 128, 256]:
#         t_mean = df1[(df1.nw == nw) & (df1.rowcol == i)].mean()

seq = df0.groupby(['nw', 'rowcol']).mean().reset_index()
par1 = df1.groupby(['nw', 'rowcol']).mean().reset_index()
par2 = df2.groupby(['nw', 'rowcol']).mean().reset_index()
par3 = df3.groupby(['nw', 'rowcol']).mean().reset_index()


# plt.figure(figsize=(4, 2))
plt.figure()
for nw in [4, 8, 16, 32, 64, 128, 256]:
    tmp1 = par1[(par1.nw == nw)]
    tmp2 = par2[(par2.nw == nw)]
    tmp3 = par3[(par3.nw == nw)]
    plt.plot(seq['rowcol'], seq['time'], 'o-b')
    plt.plot(tmp1['rowcol'], tmp1['time'], '-*r')
    plt.plot(tmp2['rowcol'], tmp2['time'], '*-.g')
    plt.plot(tmp3['rowcol'], tmp3['time'], '^--y')
    plt.show()


res0 = res0[(res0.rowcol == 256)]
res0.plot(x='rowcol', y='time')
plt.show()


res1 = res1[(res1.nw == 4)]
res1.plot()
plt.show()


res1.plot()
res2.plot()
res3.plot()

print("ciao")
