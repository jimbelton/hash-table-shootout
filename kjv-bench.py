import sys, os, subprocess, signal

best_out_of = 2
programs    = os.listdir("build")

for program in programs:
    fastest_attempt = None

    for attempt in range(best_out_of):
        proc = subprocess.Popen(['./build/'+program, "1", "kjvmark"], stdout=subprocess.PIPE)

        # wait for the program to fill up memory and spit out its "ready" message
        try:
            runtime = float(proc.stdout.readline().strip())
        except:
            runtime = 0

        ps_proc = subprocess.Popen(['ps up %d | tail -n1' % proc.pid], shell=True, stdout=subprocess.PIPE)
        nbytes = int(ps_proc.stdout.read().split()[4]) * 1024
        ps_proc.wait()

        os.kill(proc.pid, signal.SIGKILL)
        proc.wait()

        if runtime: # otherwise it crashed
            if attempt == 0:
                fastest_attempt = runtime

            elif attempt and runtime < fastest_attempt:
                fastest_attempt = runtime

    if not fastest_attempt:
        print "No run of " + program + " succeeded"
    else:
        print "%s: %f" % (program, fastest_attempt)
