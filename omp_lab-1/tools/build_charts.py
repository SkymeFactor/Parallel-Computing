from dataclasses import dataclass
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import os


SAVE_FOLDER = "data/img"


@dataclass
class PlotConfig:
    filename: str
    fig_name: str
    xlabel: str
    ylabel: str
    chart_type: str
    xscale: str = 'linear'
    yscale: str = 'linear'



def parse_file(filename: str):
    data = {}
    lines = []
    section = ''

    with open(filename) as file:
        lines = file.readlines()

    for line in lines:
        if (line.startswith('[')):
            section = line.split('[')[1].split(']')[0]
            data.update({section: []})
        else:
            if (section == ''):
                continue
            # NOTE: puts all the floats from current line
            data[section] += [float(i) for i in line.split() if i.replace('.','',1).isdigit()]
    
    return data



def plot_figure_from_file(plot_conf: PlotConfig):
    print(f'Parsing {plot_conf.filename}')
    data = parse_file(plot_conf.filename)

    if (plot_conf.chart_type == 'chart'):
        plot_chart(data, plot_conf)
    if (plot_conf.chart_type == 'bar'):
        plot_barchart(data, plot_conf)
    
    if (not os.path.isdir(SAVE_FOLDER)):
        print(f'creating directory {SAVE_FOLDER}')
        os.mkdir(SAVE_FOLDER)
    
    print('Saving ', plot_conf.fig_name + '.png')
    plt.savefig(os.path.join(SAVE_FOLDER, plot_conf.fig_name + '.png'))



def plot_chart(data: dict, plot_conf: PlotConfig):
    num_elements = len(list(data.values())[0])
    # FIXME: simplified workaround when the x-axis is unknown (always)
    y = [10 ** i * 0.000001 for i in range(num_elements, 0, -1)]
    
    fig=plt.figure(label=plot_conf.fig_name)
    ax=fig.add_subplot(111)
    ax.set_title(plot_conf.fig_name)
    ax.set_xlabel(plot_conf.xlabel)
    ax.set_ylabel(plot_conf.ylabel)
    ax.set_yscale(plot_conf.yscale)
    ax.set_xscale(plot_conf.xscale)

    for label, x in data.items():
        ax.plot(y, x, 'o', ls='-', label = label)

    plt.legend(loc=1)



def plot_barchart(data: dict, plot_conf: PlotConfig):
    keys = [ i.split(' ')[-1] for i in list(data.keys())]
    # NOTE: always takes one last element if more than one
    values = [i[-1] for i in data.values()]
    
    fig = plt.figure(label=plot_conf.fig_name)
    ax = fig.add_subplot(111)
    ax.bar(keys, values, color=sns.colors.xkcd_rgb['deep sky blue'])
    ax.set_ylim([min(values) - 0.2, max(values) + 0.2])
    ax.set_title(plot_conf.fig_name)
    ax.set_xlabel(plot_conf.xlabel)
    ax.set_ylabel(plot_conf.ylabel)
    ax.set_yscale(plot_conf.yscale)
    ax.set_xscale(plot_conf.xscale)
    ax.set_xticks([i for i in range(len(values))], labels=keys)



def main():
    files=[
        PlotConfig(
            filename = 'data/perf_test_chunk_size_dynamic.txt',
            fig_name = 'Performance from chunk_size and workload',
            xlabel = 'Epsilon',
            ylabel = 'Time (ms)',
            xscale = 'log',
            yscale = 'log',
            chart_type = 'chart'
        ),
        PlotConfig(
            filename = 'data/perf_test_chunk_size.txt',
            fig_name = 'Performance from chunk_size',
            xlabel = 'chunk_size',
            ylabel = 'Time (ms)',
            chart_type = 'bar'
        ),
        PlotConfig(
            filename = 'data/perf_test_dynamic.txt',
            fig_name = 'Performance from workload',
            xlabel = 'Epsilon',
            ylabel = 'Time (ms)',
            yscale = 'log',
            xscale = 'log',
            chart_type = 'chart'
        )
    ]

    for file in files:
        if os.path.isfile(file.filename):
            plot_figure_from_file(file)
        else:
            print(f'File {file.filename} does not exist')



if __name__ == "__main__":
    main()