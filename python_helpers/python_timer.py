import time

class Timer:
    def start(self):
        self.start_time = time.perf_counter()

    def stop(self):
        self.end_time = time.perf_counter()

    def print_elapsed_time(self):
        elapsed_time = self.end_time - self.start_time
        print("Elapsed time: {:.6f} seconds".format(elapsed_time))
