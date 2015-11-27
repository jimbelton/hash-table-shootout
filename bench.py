import os
import re
import signal
import subprocess
import sys

best_out_of = 2
programs    = os.listdir("build")
pattern     = re.compile(r'(\d+(?:\.\d+)?) (\d+)')
maxkeys     = 40*1000*1000
interval    =  2*1000*1000

# for the final run, use this:
#maxkeys  = 40*1000*1000
#interval =  2*1000*1000
#best_out_of = 3
# and use nice/ionice
# and shut down to the console
# and swapoff any swap files/partitions`

outfile = open('output', 'w')

if len(sys.argv) > 1:
    benchtypes = sys.argv[1:]
else:
    benchtypes = ('sequential', 'random', 'delete', 'sequentialstring', 'randomstring', 'deletestring')

for benchtype in benchtypes:
    for program in programs:
        fastest_attempt = []

        for attempt in range(best_out_of):
            points = []
            proc   = subprocess.Popen(['./build/' + program,  benchtype, str(maxkeys), str(interval)], stdout=subprocess.PIPE)

            for size in range(interval, maxkeys + 1, interval):
                # wait for the benchmark to output a time and amount of data memory used
                line = proc.stdout.readline()

                if not line:
                    sys.stderr.write("%s: %s %s failed to output all results\n" % (__file__, program, benchtype))
                    break

                match = pattern.match(line)

                if not match:
                    sys.stderr.write("%s: %s %s output did not contain time and memory: %s" % (__file__, program, benchtype, line))
                    break

                points.append((float(match.group(1)), int(match.group(2))))

            # Shutdown the benchmark if needed
            ps_proc = subprocess.Popen(['ps up %d | tail -n1' % proc.pid], shell=True, stdout=subprocess.PIPE)
            nbytes = int(ps_proc.stdout.read().split()[4]) * 1024
            ps_proc.wait()
            os.kill(proc.pid, signal.SIGKILL)
            proc.wait()

            # If there is output and at least as much as any previous attempt
            if len(points) > 0 and len(points) >= len(fastest_attempt):
                if len(points) > len(fastest_attempt) or points[len(points) - 1][0] < fastest_attempt[len(fastest_attempt) - 1][0]:
                    fastest_attempt = points

        if len(fastest_attempt) > 0:
            for i in range(len(fastest_attempt)):
                size = (i + 1) * interval
                outfile.write("%s,%d,%s,%d,%0.6f\n" % (benchtype, size, program, fastest_attempt[i][1], fastest_attempt[i][0]))

            print "%s %s: %d keys, %fs, %d Mbytes" % (program, benchtype, size, fastest_attempt[i][0],
                                                      fastest_attempt[i][1] / 1048576)

        else:
            print "No run of %s %s succeeded" % (program, benchtype)
