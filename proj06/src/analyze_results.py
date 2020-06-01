import argparse
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# label names
label_names = {
    (2 ** 10): '1K',
    (2 ** 11): '2K',
    (2 ** 12): '4K',
    (2 ** 13): '8K',
    (2 ** 14): '16K',
    (2 ** 15): '32K',
    (2 ** 16): '64K',
    (2 ** 17): '128K',
    (2 ** 18): '256K',
    (2 ** 19): '512K',
    (2 ** 20): '1M',
    (2 ** 21): '2M',
    (2 ** 22): '4M',
    (2 ** 23): '8M'
}


def dump_table(file_name):
    # load the dataframe
    sim_df = pd.read_csv('../data/{}.csv'.format(file_name))

    # find unique work sizes
    unique_work_sizes = sim_df['local'].unique()

    # find unique array sizes
    unique_array_sizes = sim_df['global'].unique()
    unique_array_sizes = sorted(unique_array_sizes)
    unique_array_sizes = [label_names[array_size] for array_size in unique_array_sizes]

    # merge columns
    series_list = []
    for work_size in unique_work_sizes:
        table_df = sim_df[sim_df['local'] == work_size]
        table_df = table_df.sort_values(by='global')
        performances = table_df['performance'].values
        series = pd.Series(performances, index=unique_array_sizes, name=str(work_size))
        series_list.append(series)
    combine_df = pd.concat(series_list, axis=1)

    # save combined dataframe
    combine_df.index.name = 'performance'
    combine_df.to_csv('../data/{}-simulation.csv'.format(file_name), float_format='%.2f')


def plot_graphs(file_name, title):
    # load the dataframe
    sim_df = pd.read_csv('../data/{}.csv'.format(file_name))

    # plot details
    plot_configs = [
        ('local', 'Local Work Size', 'global', 'Array Size', '{}-local.png'.format(file_name)),
        ('global', 'Array Size', 'local', 'Local Work Size', '{}-global.png'.format(file_name))
    ]

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

        plt.legend(title=legend, loc='right')
        plt.ylabel("Performance (Giga-Mults/Second)")
        plt.xlabel(x_label)
        plt.title("Performance of OpenCL Array {}".format(title))
        plt.savefig("../data/{}".format(png))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Process simulation.')
    parser.add_argument('str', type=str, help='simulation file name')
    args = parser.parse_args()

    input = args.str
    strs = input.split()
    fName = strs[0]
    plotName = ' '.join(strs[1:])

    dump_table(fName)
    plot_graphs(fName, plotName)
