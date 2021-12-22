###################
# $ python3 plot.py [plot_type] [result_name] [filter_name_1] [filter_name_2] [filter_name_3]
###################
import sys
import re
import matplotlib.pyplot as plt

arguments = {}
f = open("../default_config.txt", 'r')
while True:
    line = f.readline().split()
    if len(line) < 2:
        break
    arguments[line[0]] = line[1]
round = int(arguments["round"])

n = 0
plot_type = int(sys.argv[1])
result_name = sys.argv[2]
colors = ['red', 'green', 'blue', 'black', 'purple']
names = []
insert = []
query = []
remove = []

for filter in sys.argv[3:]:
    n += 1
    lines = []
    for id in range(round):
        f = open("../log/" + result_name + "_" + filter + "#" + str(id) + ".txt", 'r')
        if id == 0:
            names.append(f.readline().split()[0])
            insert.append([])
            query.append([])
            remove.append([])
        while True:
            line = f.readline().split()
            if(line[0] == 'Procedure:'):
                break
        lines.append([])
        while True:
            line = re.split("[\s\n=,][\s\n=,]*", f.readline())
            if len(line) <= 1:
                break
            else:
                lines[-1].append(line)
        f.close()
    line_cnt = 0
    while True:
        line_type = None
        status = {}
        empty = True
        for r in range(len(lines)):
            if line_cnt < len(lines[r]):
                line = lines[r][line_cnt]
                if empty:
                    line_type = line[0]
                    for i in range(1, len(line)-1, 2):
                        status[line[i]] = (float(line[i+1]), 1)
                    empty = False
                else:
                    for i in range(1, len(line)-1, 2):
                        status[line[i]] = (status[line[i]][0] + float(line[i+1]), status[line[i]][1] + 1)
        if empty:
            break
        for key in status:
            status[key] = status[key][0] / status[key][1]
        if line_type[0] == 'I':
            if len(insert[-1]) < 1 or status['Load_factor'] > insert[-1][-1]['Load_factor']:
                insert[-1].append(status)
        elif line_type[0] == 'Q':
            if len(query[-1]) < 1 or status['Load_factor'] > query[-1][-1]['Load_factor']:
                query[-1].append(status)
        else:
            if len(remove[-1]) < 1 or status['Load_factor'] > remove[-1][-1]['Load_factor']:
                remove[-1].append(status)
        line_cnt = line_cnt + 1


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
fig.savefig('../log/' + result_name + '.png', dpi=1000)