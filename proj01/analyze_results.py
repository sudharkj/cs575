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
results_df = sim_df[['Trials', 'Threads', 'MaxPerformance', 'Probability']]
results_df = results_df.sort_values(['Trials', 'Threads'])
results_df.to_csv('data/output/results.csv', index=False)

# plot MegaTrials/Second vs # of Trials
unique_threads = results_df['Threads'].unique()
unique_threads.sort()
plt.clf()
for threads in unique_threads:
    df = results_df[results_df['Threads'] == threads]
    df = df.sort_values('Trials')
    x = df['Trials'].values
    y = df['MaxPerformance'].values
    plt.plot(x, y, marker='o', label='{} threads'.format(threads))
plt.xlabel('# of Monte Carlo Trials')
plt.ylabel('MegaTrials/Second')
plt.title('Monte Carlo Performance')
plt.legend()
# plt.show()
plt.savefig('data/output/mts_vs_trials.png')

# plot MegaTrials/Second vs # of Threads
unique_trials = results_df['Trials'].unique()
unique_trials.sort()
plt.clf()
for trials in unique_trials:
    df = results_df[results_df['Trials'] == trials]
    df = df.sort_values('Threads')
    x = df['Threads'].values
    y = df['MaxPerformance'].values
    plt.plot(x, y, marker='o', label='{} trials'.format(trials))
plt.xlabel('# of Threads')
plt.ylabel('MegaTrials/Second')
plt.title('Monte Carlo Performance')
plt.legend()
# plt.show()
plt.savefig('data/output/mts_vs_threads.png')

# print estimated probability
print('Estimate of actual probability = {}'.format(results_df['Probability'].values[-1]))

# calculate fp
t1 = results_df[results_df['Threads'] == 1]['MaxPerformance'].values[-1]
t4 = results_df[results_df['Threads'] == 4]['MaxPerformance'].values[-1]
speedup = t4 / t1
fp = (4/3) * (1 - (1/speedup))
print('Parallel Fraction = {}'.format(fp))
