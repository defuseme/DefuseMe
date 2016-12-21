import threading

def do_every(interval,worker_func,cond_func=None,iterations=0):
    if cond_func and not cond_func():
        return
    if iterations != 1:
        threading.Timer(
                interval,
                do_every, [interval,worker_func,cond_func, 0 if iterations==0 else iterations-1]
                ).start()
    worker_func()
