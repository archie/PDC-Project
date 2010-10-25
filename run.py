import sys
import pickle
import time
import os, glob
import subprocess 

path = []
files = []

if len(sys.argv) > 1:
    if sys.argv[1].endswith('.txt'):
        [files.append(x) for x in sys.argv[1:]]
    else:
        files = glob.glob(os.path.join(sys.argv[1], '*.txt'))
else:
    path = 'cases/ex/'
    files = glob.glob(os.path.join(path, '*.txt'))


times = []
times_all = {}
run_no_times = 5

for sudoku in files:
    one_sum = 0
    print "%s" % sudoku
    for x in range(1,run_no_times+1):
        output,error = subprocess.Popen(['time', './omp', sudoku], 
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.PIPE).communicate()
        [real] = error.strip().split()[:1]
        print "\t%d,%f" % (x,float(real))
        times_all[(sudoku,x)] = float(real)
        one_sum = one_sum + float(real)

    one_avg = one_sum/run_no_times
    print "%s,%f" % (sudoku, one_avg)
    times.append(one_avg)
        

total = sum(times)
avg = total/len(times)

#print 'Average: %f' % avg

#Log runs
storage = {}
storage['time'] = time.localtime()
storage['path'] = path
storage['average'] = avg
storage['detailed'] = times_all
logfile = open(str(time.time()) + '.log', 'w')
pickle.dump(storage, logfile)
logfile.close()
