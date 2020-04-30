import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# load the dataframe
df = pd.read_csv('../data/results.csv')

# find the base performances
base_df = df[(df['simd'] == 0) & (df['core'] == 0)]
# save only the performances
base_df = base_df[['performance']]
# repeat each row 9 times immediately before the next row
base_df = base_df.loc[base_df.index.repeat(9)]
# reset index
base_df = base_df.reset_index(drop=True)

# get the required simulation dataframe
sim_df = df[(df['simd'] != 0) | (df['core'] != 0)]
# reset index for merging
sim_df = sim_df.reset_index(drop=True)

# figure out the speedups
speedup = sim_df['performance'] / base_df['performance']
# create dataframe
speedup_df = speedup.to_frame(name='speedup')

# simulation types
sim_types = [
    'SIMD alone',
    '1 core alone',
    '2 core alone',
    '4 core alone',
    '8 core alone',
    'SIMD + 1 core',
    'SIMD + 2 core',
    'SIMD + 4 core',
    'SIMD + 8 core'
]
# simulation order
sim_order = []
for _ in range(9):
    sim_order.extend(sim_types)
# simulation type dataframe
types_df = pd.DataFrame(data=sim_order, columns=['type'])

# create the results dataframe
results_df = pd.concat([types_df, sim_df[['size', 'performance']], speedup_df], axis=1)
results_df = results_df.sort_values(by='size')
# save to basic necessities
simple_df = results_df[results_df['type'] == sim_types[0]]
simple_df = simple_df[['size', 'performance', 'speedup']]
simple_df = simple_df.sort_values(by='size')
simple_df.to_csv("../data/simulation.csv", index=False, float_format='%.2f')
# save extra credits
results_df.to_csv("../data/extra.csv", index=False, float_format='%.2f')

# plot the results
plt.clf()
plt.figure(figsize=(11, 5))
sns.set_style("darkgrid")
x = simple_df['size'].values
x = x / (10 ** 6)
y = simple_df['speedup'].values
plt.plot(x, y, marker='o')
plt.ylabel("Speedup")
plt.xlabel("Array Size (M)")
plt.title("Speedup For Vectorized Array Multiplication/Reduction (SIMD Alone)")
plt.savefig("../data/simulation.png")

# plot the extra credit results
plt.clf()
plt.figure(figsize=(11, 5))
sns.set_style("darkgrid")
for sim_type in sim_types:
    plot_df = results_df[results_df['type'] == sim_type]
    plot_df = plot_df.sort_values(by='size')
    x = plot_df['size'].values
    x = x / (10 ** 6)
    y = plot_df['speedup'].values
    plt.plot(x, y, marker='o', label=sim_type)
plt.legend(title='Type', loc='upper right')
plt.ylabel("Speedup")
plt.xlabel("Array Size (M)")
plt.title("Speedup For Vectorized Array Multiplication/Reduction")
plt.savefig("../data/extra.png")
