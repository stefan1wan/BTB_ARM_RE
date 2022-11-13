

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import numpy as np
import matplotlib.pyplot as plt

# read map from file
x_branch_num = []
y_align = []
z_btb_size = []
# draw map
with open('btb_miss_pred_map.txt') as f:
    content = f.readlines()
    for line in content:
        if not line:
            continue
        v = line.split()
        if len(v) <2:
            continue
        print(v)
        Branch_size = str(int(int(v[0])/1024))+"K"
        Branch_mis_pred_rate = []
        for i in range(1, len(v)):
            Branch_mis_pred_rate.append(float(v[i]))
        x_branch_num.append(Branch_size)
        z_btb_size.append(Branch_mis_pred_rate)
    y_align = [str(2**(i+3)) for i in range(0, len(Branch_mis_pred_rate))]
    f.close()

x_lable = x_branch_num
y_lable = y_align
arr = np.array(z_btb_size).transpose()

fig, ax = plt.subplots()
im = ax.imshow(arr)

# Show all ticks and label them with the respective list entries
ax.set_xticks(np.arange(len(x_lable)), labels=x_lable)
ax.set_yticks(np.arange(len(y_lable)), labels=y_lable)


# Rotate the tick labels and set their alignment.
plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
        rotation_mode="anchor")

# Loop over data dimensions and create text annotations.
for i in range(len(y_lable)):
    for j in range(len(x_lable)):
        text = ax.text(j, i, arr[i][j],
                    ha="center", va="center", color="w")


item = ax.set_title("Branch misprediction rate (branch mispredictions/B)")
# set font size: https://stackoverflow.com/questions/3899980/how-to-change-the-font-size-on-a-matplotlib-plot
axis_font = {'fontname':'Arial', 'size':'14'}

ax.set_xlabel("B", **axis_font)
ax.set_ylabel("N", **axis_font)

fig.tight_layout()
plt.show()
