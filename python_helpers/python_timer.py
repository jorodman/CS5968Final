import time

class Timer:
    def __init__(self, label = 'No label'):
        self.label = label

    def start(self):
        self.start_time = time.perf_counter()

    def stop(self):
        self.end_time = time.perf_counter()

    def print_elapsed_time(self):
        elapsed_time = self.end_time - self.start_time
        # print(self.label + ': ' + str(round(elapsed_time, 5)) + '\n')
        print(str(round(elapsed_time, 5)))
