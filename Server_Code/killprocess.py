from psutil import process_iter
from signal import SIGKILL # or SIGKILL
for proc in process_iter():
    for conns in proc.connections(kind='inet'):
        if conns.laddr.port == XXXX:
            proc.send_signal(SIGKILL) # or SIGKILL