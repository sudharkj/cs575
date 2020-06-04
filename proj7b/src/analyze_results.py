import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns


def plot_sine_wave():
    # get required data
    shifts = [ind for ind in range(1, 513)]
    sine_df = sim_df.iloc[0]
    sine_df = sine_df[shifts]

    # reset plot configuration
    plt.clf()
    sns.set()
    plt.figure(figsize=(11, 5))
    # draw scatter-plot
    sns.scatterplot(shifts, sine_df.values)
    plt.xticks(list(range(-40, 562, 40)))
    plt.xlabel('Shift')
    plt.ylabel('Sums')
    plt.title('Autocorrelation')
    # save the figure
    plt.savefig("../data/sine-wave.png")
    # close the plot
    plt.close()


def get_results_df():
    # no parallel result
    none_df = sim_df[sim_df['mode'] == 'None']
    # open-mp parallel results
    open_mp_df = sim_df[sim_df['mode'] == 'OpenMP']
    open_mp_1_df = open_mp_df[open_mp_df['param'] == 1]
    open_mp_df = open_mp_df[open_mp_df['param'] != 1]
    open_mp_df = open_mp_df.sort_values(by='performance', axis=0, ascending=False)
    open_mp_df = open_mp_df.iloc[[0]]
    open_mp_df = pd.concat([open_mp_1_df, open_mp_df])
    # simd parallel results
    simd_df = sim_df[sim_df['mode'] == 'SIMD']
    mp_simd_df = sim_df[sim_df['mode'] == 'OpenMP-SIMD']
    mp_simd_df = mp_simd_df.sort_values(by='performance', axis=0, ascending=False)
    mp_simd_df = mp_simd_df.iloc[[0]]
    simd_df = pd.concat([simd_df, mp_simd_df])
    # open-cl parallel results
    open_cl_df = sim_df[sim_df['mode'] == 'OpenCL']
    open_cl_df = open_cl_df.sort_values(by='performance', axis=0, ascending=False)
    open_cl_df = open_cl_df.iloc[[0]]
    # combined result sorted
    result_df = pd.concat([none_df, simd_df, open_mp_df, open_cl_df])
    result_df = result_df.sort_values(by='performance', axis=0)
    return result_df


def get_formated_df(data_df):
    x = ["{}-{}".format(mode, param) for mode, param in data_df[['mode', 'param']].values]
    y = data_df['performance'].values
    data = {'Approach': x, 'Performance': y}
    return pd.DataFrame(data=data)


def plot_performances(data_df, filename, figsize):
    # reset plot configuration
    plt.clf()
    sns.set()
    # draw bar-plot
    plt.figure(figsize=figsize)
    sns.barplot(x='Approach', y='Performance', data=data_df)
    plt.xlabel('Approach')
    plt.ylabel('Performance (Giga-Mults/Sec)')
    plt.title('Performance Comparison')
    # save the figure
    plt.savefig(filename)
    # close the plot
    plt.close()


if __name__ == '__main__':
    # load the dataframe
    sim_df = pd.read_csv("../data/simulations.csv")

    # plot sine wave
    plot_sine_wave()

    # trim dataframe
    columns = ['mode', 'param', 'performance']
    sim_df = sim_df[columns]

    # plot overall performances
    formated_df = get_formated_df(sim_df)
    formated_df.to_csv("../data/overall-comparison.csv", index=False, float_format='%0.2f')
    plot_performances(formated_df, "../data/overall-comparison.png", (75, 15))

    # get results
    results_df = get_results_df()
    results_df = get_formated_df(results_df)
    results_df.to_csv("../data/comparison.csv", index=False, float_format='%0.2f')
    plot_performances(results_df, "../data/comparison.png", (15, 15))
