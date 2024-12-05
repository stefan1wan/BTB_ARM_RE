
# read map from file

x_branch_num = []
y_align = []
z_btb_size = []
# draw map
# with open('btb_miss_pred_map.txt') as f:
with open('btb_miss_pred_map_pi.txt') as f:
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
    y_align = [str(2**(i+2)) for i in range(0, len(Branch_mis_pred_rate))]
    f.close()
        

from mpl_toolkits import mplot3d
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import numpy as np
import matplotlib.pyplot as plt
fig = plt.figure()
#创建3d绘图区域
ax = plt.axes(projection='3d')


_x = np.arange(len(x_branch_num))
_y = np.arange(len(y_align))
_xx, _yy = np.meshgrid(_x, _y)
print(_xx, _yy)
x, y = _xx.ravel(), _yy.ravel()
top = np.array(z_btb_size).transpose().ravel()
bottom = np.zeros_like(top)

colors = plt.cm.jet(top.flatten()/float(top.max()))

width = 1
depth = 1

from mpl_toolkits.axes_grid1 import make_axes_locatable


scat_plot = ax.bar3d(y, x, bottom, width, depth, top, shade=True, color=colors)

print(x_branch_num)
print(y_align)
ax.w_yaxis.set_ticks(_x)
ax.w_xaxis.set_ticks(_y)
ax.w_yaxis.set_ticklabels(x_branch_num)
ax.w_xaxis.set_ticklabels(y_align)
ax.set_ylabel('B')
ax.set_xlabel('N')
ax.set_title('Branch mis-predict rate (mis-predict branch number/B)')

plt.show()