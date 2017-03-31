# from turkey import *
#
# import docker
# client = docker.from_env()
# a = client.containers.run('danielsuo/parsec:prod', ['-a', 'run', '-p', 'blackscholes'], detach=True)
# b = client.containers.run('danielsuo/parsec:prod', ['-a', 'run', '-p', 'blackscholes', '-i', 'simlarge'], detach=True)
# b.wait()
# a.wait()
# print(a.logs())
# print("b: " + b.logs())

from bcc import BPF
from time import sleep

# load BPF program
b = BPF(text="""
#include <uapi/linux/ptrace.h>
#include <linux/blkdev.h>
BPF_HISTOGRAM(dist);
int kprobe__blk_account_io_completion(struct pt_regs *ctx, struct request *req)
{
	dist.increment(bpf_log2l(req->__data_len / 1024));
	return 0;
}
""")

# header
print("Tracing... Hit Ctrl-C to end.")

# trace until Ctrl-C
try:
	sleep(99999999)
except KeyboardInterrupt:
	print

# output
b["dist"].print_log2_hist("kbytes")

# run_json('run/2017-03-21-a.json')
# run_jobs(['asdf', 'asdf'])
