import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

df = pd.read_csv("../data/results.csv")

cols = ['Month', 'Precipitation', 'Temperature', 'Height', 'Deers', 'Humans']
results_df = df[cols]

# convert height units from inches to cms
results_df['Height'] = df['Height'] * 2.54
# convert precipitation units from inches to cms
results_df['Precipitation'] = df['Precipitation'] * 2.54
# convert temperature from Fahrenheit to Celsius
results_df['Temperature'] = 5 * (df['Temperature'] - 32) / 9
# get month number from year and month values
results_df['Month'] = (df['Year'] % 2020) * 12 + df['Month'] + 1
# sort based on month number
results_df.sort_values(by='Month')
# save results before
results_df.to_csv("../data/simulation.csv", index=False, float_format='%.2f')

# rename columns for plot
new_cols = ['Months', 'Temperature ($^\circ$C)', 'Precipitation (cm)', 'Deers', 'Height (cm)', 'Humans']
results_df = results_df.rename(columns=dict((c, nc) for c, nc in zip(cols, new_cols)))

# plot the results
plt.clf()
plt.figure(figsize=(10, 5))
sns.set_style("darkgrid")
sns.lineplot(data=results_df[new_cols[1:]])
plt.legend(loc='upper right')
plt.xlabel("Months")
plt.ylabel("Values")
plt.title("Simulation")
plt.savefig("../data/simulation.png")
