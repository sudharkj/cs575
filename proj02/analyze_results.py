import os
import pandas as pd
import matplotlib.pyplot as plt

li = []
for filename in os.listdir('data/input'):
    df = pd.read_csv("data/input/{filename}".format(filename=filename))
    li.append(df)
sim_df = pd.concat(li, axis=0, ignore_index=True)

# save results to a table
sim_df = sim_df[sim_df['Iteration'] == 10]
results_df = sim_df[['Threads', 'MegaNodes', 'MaxPerformance', 'MaxVolume']]
results_df = results_df.sort_values(['Threads', 'MegaNodes'])
results_df.to_csv('data/output/results.csv', index=False)

# plot MegaHeightsComputed/Second vs # of Nodes
unique_threads = results_df['Threads'].unique()
unique_threads.sort()
plt.clf()
for threads in unique_threads:
    df = results_df[results_df['Threads'] == threads]
    df = df.sort_values('MegaNodes')
    x = df['MegaNodes'].values
    y = df['MaxPerformance'].values
    plt.plot(x, y, marker='o', label='{} threads'.format(threads))
plt.xlabel('# of MegaNodes')
plt.ylabel('MegaHeightsComputed/Second')
plt.title('Numeric Integration')
plt.legend()
# plt.show()
plt.savefig('data/output/mhcps_vs_mnodes.png')

# plot MegaHeightsComputed/Second vs # of Nodes
unique_threads = results_df['Threads'].unique()
unique_threads.sort()
plt.clf()
for threads in unique_threads:
    df = results_df[results_df['Threads'] == threads]
    df = df.sort_values('MegaNodes')
    x = df['MegaNodes'].values
    y = df['MaxPerformance'].values
    y /= 40
    y = y ** 15
    plt.plot(x, y, marker='o', label='{} threads'.format(threads))
plt.xlabel('# of MegaNodes')
plt.ylabel('(MegaHeightsComputed/Second / 40) ** 15')
plt.title('Numeric Integration')
plt.legend()
# plt.show()
plt.savefig('data/output/mhcps_vs_mnodes_modified.png')

# plot MegaHeightsComputed/Second vs # of Threads
unique_nodes = results_df['MegaNodes'].unique()
unique_nodes.sort()
plt.clf()
for nodes in unique_nodes:
    df = results_df[results_df['MegaNodes'] == nodes]
    df = df.sort_values('Threads')
    x = df['Threads'].values
    y = df['MaxPerformance'].values
    plt.plot(x, y, marker='o', label='{} mega-nodes'.format(nodes))
plt.xlabel('# of Threads')
plt.ylabel('MegaHeightsComputed/Second')
plt.title('Numeric Integration')
plt.legend()
# plt.show()
plt.savefig('data/output/mhcps_vs_threads.png')

# plot MegaHeightsComputed/Second vs # of Threads
unique_nodes = results_df['MegaNodes'].unique()
unique_nodes.sort()
plt.clf()
for nodes in unique_nodes:
    df = results_df[results_df['MegaNodes'] == nodes]
    df = df.sort_values('Threads')
    x = df['Threads'].values
    y = df['MaxPerformance'].values
    y /= 40
    y = y ** 15
    plt.plot(x, y, marker='o', label='{} mega-nodes'.format(nodes))
plt.xlabel('# of Threads')
plt.ylabel('(MegaHeightsComputed/Second / 40) ** 15')
plt.title('Numeric Integration')
plt.legend()
# plt.show()
plt.savefig('data/output/mhcps_vs_threads_modified.png')

# print estimated probability
print('Estimate of actual volume')
print(results_df['MaxVolume'].describe())

# calculate fp
p1 = results_df[results_df['Threads'] == 1]['MaxPerformance'].values[-1]
print("Performance of 1 thread: {:.2f}".format(p1))
p8 = results_df[results_df['Threads'] == 8]['MaxPerformance'].values[-1]
print("Performance of 8 thread: {:.2f}".format(p8))
speedup = p8 / p1
print("Speedup with 8 thread: {:.2f}".format(speedup))
fp = (8/7) * (1 - (1/speedup))
print('Parallel Fraction = {:.2f}'.format(fp))
max_speedup = 1 / (1 - fp)
print('Max Speedup = {:.2f}'.format(max_speedup))
