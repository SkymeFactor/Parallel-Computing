import numpy as np
import cv2 as cv
import os, sys
from matplotlib import pyplot as plt


SAVE_FOLDER = 'data'

def plot_img_hist(colors: np.ndarray, counts: np.ndarray, name: str):
    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.bar(colors, counts, width=1.0)
    ax.set_title(f"PGM histogram for {name}")
    ax.set_xlabel("Color")
    ax.set_ylabel("Quantity")
    ax.set_xlim([0, 255])
    
    plt.savefig(os.path.join(SAVE_FOLDER, 'Hist_from_' + name.split('/')[-1] + '.png'))


def main():
    # Read the image and build its histogram
    filename = sys.argv[1]
    img = cv.imread(filename, 0).flatten()
    counts = np.zeros(256)
    for i in img:
        counts[i] += 1
    plot_img_hist(np.argwhere(counts != 0).flatten(), counts[counts != 0], filename)

    # Read the binary array and compare it to histogram from the above code
    if len(sys.argv) < 3:
        filename = "data/example.bin"
    else:
        filename = sys.argv[2]
    lst = np.fromfile(filename, np.int32, -1, '')
    lst.resize(256)
    
    plot_img_hist(np.argwhere(lst != 0).flatten(), lst[lst != 0], filename)
    
    print((counts == lst).all())


if __name__ == "__main__":
    main()