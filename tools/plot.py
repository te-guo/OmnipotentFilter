###################
# $ python3 plot.py ../log/[file_1] ../log/[file_2] ../log/[file_3]
###################
import sys
import re
import matplotlib.pyplot as plt

n = 0
colors = ['red', 'green', 'blue', 'black', 'purple']
names = []
insert = []
query = []

for file in sys.argv[1:]:
    f = open(file, 'r')
    names.append(f.readline().split()[0])
    insert.append([])
    query.append([])
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
            insert[-1].append(status)
        else:
            query[-1].append(status)
    f.close()
    n += 1


plt.figure()
plt.title('Insert Throughput')
plt.xlabel('Load factor')
plt.ylabel('Throughput')
plt.xlim(xmin=0,xmax=1)
#plt.ylim(ymin=0)
#plt.grid(linestyle='--')
for i in range(n):
    x = [status['Load_factor'] for status in insert[i]]
    y = [status['Throughput'] for status in insert[i]]
    plt.plot(x, y, color = colors[i], linewidth=1, linestyle='-', label=names[i])
plt.legend()
plt.savefig('../log/Insert Throughput.png', dpi=1000)


plt.figure()
plt.title('Query Throughput')
plt.xlabel('Load factor')
plt.ylabel('Throughput')
plt.xlim(xmin=0,xmax=1)
#plt.ylim(ymin=0)
#plt.grid(linestyle='--')
for i in range(n):
    x = [status['Load_factor'] for status in query[i]]
    y = [status['Throughput'] for status in query[i]]
    plt.plot(x, y, color = colors[i], linewidth=1, linestyle='-', label=names[i])
plt.legend()
plt.savefig('../log/Query Throughput.png', dpi=1000)


plt.figure()
plt.title('FPR')
plt.xlabel('Load factor')
plt.ylabel('FPR')
plt.xlim(xmin=0,xmax=1)
#plt.ylim(ymin=0)
#plt.grid(linestyle='--')
for i in range(n):
    x = [status['Load_factor'] for status in query[i]]
    y = [status['FPR'] for status in query[i]]
    plt.plot(x, y, color = colors[i], linewidth=1, linestyle='-', label=names[i])
plt.legend()
plt.savefig('../log/FPR.png', dpi=1000)