import queue

class ScreenDriver:
    NUM_COLS = 40
    NUM_LINES = 24

    def __init__(self, addr_start):
        self.addr_start = addr_start
        self.addr_end = addr_start + self.NUM_COLS * self.NUM_LINES
        self.update_queue = queue.Queue()
        self.screen = Screen(self.update_queue, self.NUM_COLS, self.NUM_LINES)

    def tick(self, addr, data, rwb):
        if rwb or addr < self.addr_start or addr > self.addr_end:
            return

        shifted_addr = addr - self.addr_start
        self.update_queue.put((shifted_addr, data))
        self.screen.update()

    def draw(self):
        self.screen.draw()

class Screen:
    CHAR_W = 16
    CHAR_H = 20

    def __init__(self, update_queue, num_cols, num_lines):
        self.update_queue = update_queue        
        self.num_lines = num_lines
        self.num_cols = num_cols
        self.memory = [ord(' ')] * self.num_lines * self.num_cols
        
    def update(self):
        try:
            addr, data = self.update_queue.get_nowait()
            self.memory[addr] = data
            self.draw()
        except queue.Empty:
            pass

    def draw(self):
        print('+-' + '-' * self.num_cols + '-+')
        print('| ',end='')

        for i in range(self.num_lines * self.num_cols):
            char = self.memory[i]
            print(chr(char), end='')
            if i != 0 and (i+1) % self.num_cols == 0:
                print(' |')
                if i + 1 < self.num_lines * self.num_cols:
                    print('| ',end='')
        print('+-' + '-' * self.num_cols + '-+')