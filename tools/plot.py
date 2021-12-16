###################
# $ python3 plot.py [output_name] ../log/[input_1] ../log/[input_2] ../log/[input_3]
###################
import sys
import re
import matplotlib.pyplot as plt

n = 0
colors = ['red', 'green', 'blue', 'black', 'purple']
names = []
insert = []
query = []
remove = []

for file in sys.argv[2:]:
    f = open(file, 'r')
    names.append(f.readline().split()[0])
    insert.append([])
    query.append([])
    remove.append([])
    while True:
        line = f.readline().split()
        if(line[0] == 'Procedure:'):
            break
    while True:
        line = re.split("[\s\n=,][\s\n=,]*", f.readline())
        if len(line) <= 1:
            break
        status = {}
        for i in range(1, len(line)-1, 2):
            status[line[i]] = float(line[i+1])
        if line[0][0] == 'I':
            if len(insert[-1]) < 1 or status['Load_factor'] > insert[-1][-1]['Load_factor']:
                insert[-1].append(status)
        elif line[0][0] == 'Q':
            if len(query[-1]) < 1 or status['Load_factor'] > query[-1][-1]['Load_factor']:
                query[-1].append(status)
        else:
            if len(remove[-1]) < 1 or status['Load_factor'] > remove[-1][-1]['Load_factor']:
                remove[-1].append(status)
    f.close()
    n += 1


fig = plt.figure()

subfig = fig.add_subplot(2, 3, 1)
subfig.set_title('Insert Throughput', fontsize=8)
subfig.set_xlabel('Load factor', fontsize=5)
subfig.set_ylabel('Throughput (M/s)', fontsize=5)
subfig.tick_params(axis='both', labelsize=5)
for i in range(n):
    x = [status['Load_factor'] for status in insert[i]]
    y = [status['Throughput']/1e6 for status in insert[i]]
    subfig.plot(x, y, color = colors[i], linewidth = 0.8, linestyle='-', label=names[i])
subfig.set_xlim(xmin=0,xmax=1)
subfig.set_ylim(ymin=0)
subfig.legend(fontsize=5)


subfig = fig.add_subplot(2, 3, 2)
subfig.set_title('Query Throughput', fontsize=8)
subfig.set_xlabel('Load factor', fontsize=5)
subfig.set_ylabel('Throughput (M/s)', fontsize=5)
subfig.tick_params(axis='both', labelsize=5)
for i in range(n):
    x = [status['Load_factor'] for status in query[i]]
    y = [status['Throughput']/1e6 for status in query[i]]
    subfig.plot(x, y, color = colors[i], linewidth = 0.8, linestyle='-', label=names[i])
subfig.set_xlim(xmin=0,xmax=1)
subfig.set_ylim(ymin=0)
subfig.legend(fontsize=5)


subfig = fig.add_subplot(2, 3, 3)
subfig.set_title('Remove Throughput', fontsize=8)
subfig.set_xlabel('Load factor', fontsize=5)
subfig.set_ylabel('Throughput (M/s)', fontsize=5)
subfig.tick_params(axis='both', labelsize=5)
for i in range(n):
    x = [status['Load_factor'] for status in remove[i]]
    y = [status['Throughput']/1e6 for status in remove[i]]
    subfig.plot(x, y, color = colors[i], linewidth = 0.8, linestyle='-', label=names[i])
subfig.set_xlim(xmin=0,xmax=1)
subfig.set_ylim(ymin=0)
subfig.legend(fontsize=5)


subfig = fig.add_subplot(2, 3, 4)
subfig.set_title('FPR', fontsize=8)
subfig.set_xlabel('Load factor', fontsize=5)
subfig.set_ylabel('FPR (%)', fontsize=5)
subfig.tick_params(axis='both', labelsize=5)
for i in range(n):
    x = [status['Load_factor'] for status in query[i]]
    y = [status['FPR']*100 for status in query[i]]
    subfig.plot(x, y, color = colors[i], linewidth = 0.8, linestyle='-', label=names[i])
subfig.set_xlim(xmin=0,xmax=1)
subfig.set_ylim(ymin=0)
subfig.legend(fontsize=5)


subfig = fig.add_subplot(2, 3, 5)
subfig.set_title('Bit Per Key', fontsize=8)
subfig.set_xlabel('Load factor', fontsize=5)
subfig.set_ylabel('Bit/Key', fontsize=5)
subfig.tick_params(axis='both', labelsize=5)
for i in range(n):
    if names[i] == 'Morton':
        continue
    x = [status['Load_factor'] for status in insert[i]]
    y = [status['BPK'] for status in insert[i]]
    subfig.plot(x, y, color = colors[i], linewidth = 0.8, linestyle='-', label=names[i])
subfig.set_xlim(xmin=0,xmax=1)
subfig.set_ylim(ymin=0)
subfig.legend(fontsize=5)

#fig.suptitle('Result', fontsize=9)
fig.tight_layout(pad=0.7, w_pad=0.7, h_pad=0.7)
fig.savefig('../log/' + sys.argv[1] + '.png', dpi=1000)