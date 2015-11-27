import os
import re
import signal
import subprocess
import sys

best_out_of = 2
programs    = os.listdir("build")
pattern     = re.compile(r'(\d+(?:\.\d+)?) (\d+)')

for program in programs:
    fastest_attempt = None

    for attempt in range(best_out_of):
        # The two "0" parameters just make it easy for the benchmark to parse its arguments
        proc    = subprocess.Popen(['./build/' + program, "kjvmark", "0", "0"], stdout=subprocess.PIPE)
        runtime = None

        # wait for the benchmark to output the time and amount of data memory used
        line = proc.stdout.readline()

        if not line:
            sys.stderr.write("%s: %s kjvmark failed to output results\n" % (__file__, program))
        else:
            match = pattern.match(line)

            if not match:
                sys.stderr.write("%s: %s kjvmark output did not contain time and memory: %s" % (__file__, program, line))
            else:
                runtime = float(match.group(1))
                data    = int(match.group(2))

        ps_proc = subprocess.Popen(['ps up %d | tail -n1' % proc.pid], shell=True, stdout=subprocess.PIPE)
        nbytes = int(ps_proc.stdout.read().split()[4]) * 1024
        ps_proc.wait()

        os.kill(proc.pid, signal.SIGKILL)
        proc.wait()

        if runtime: # otherwise it crashed
            if not fastest_attempt or runtime < fastest_attempt:
                fastest_attempt = runtime
                fastest_data    = data

    if len(sys.argv) > 1 and sys.argv[1] == "-c":
        if fastest_attempt:
            print "%s, %f, %d" % (program, fastest_attempt, fastest_data)

    else:
        if not fastest_attempt:
            print "No run of " + program + " succeeded"
        else:
            print "%s: %fs, %d Kbytes" % (program, fastest_attempt, fastest_data / 1024)
