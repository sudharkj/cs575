import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# load the dataframe
sim_df = pd.read_csv('../data/simulations.csv')

# plot details
plot_configs = [
    ('size', 'Block Sizes', 'trials', '# of Trials', 'size.png'),
    ('trials', '# of Trials', 'size', 'Block Sizes', 'trials.png')
]

# label names
label_names = {
    1024 * (2 ** 4): '16K',
    1024 * (2 ** 5): '32K',
    1024 * (2 ** 6): '64K',
    1024 * (2 ** 7): '128K',
    1024 * (2 ** 8): '256K',
    1024 * (2 ** 9): '512K',
    1024 * (2 ** 10): '1M'
}

# plot the results
for column, legend, x_axis, x_label, png in plot_configs:
    unique_values = sim_df[column].unique()
    unique_values.sort()

    plt.clf()
    plt.figure(figsize=(11, 5))
    sns.set_style("darkgrid")
    for value in unique_values:
        plot_df = sim_df[sim_df[column] == value]
        plot_df = plot_df.sort_values(by=x_axis)

        x = plot_df[x_axis].values
        y = plot_df['performance'].values
        label = label_names[value] if value in label_names else "{}".format(value)

        plt.plot(x, y, marker='o', label=label)

    plt.legend(title=legend, loc='upper right')
    plt.ylabel("Performance (Mega-Trials/Second)")
    plt.xlabel(x_label)
    plt.title("Performance of CUDA Monte Carlo")
    plt.savefig("../data/{}".format(png))
