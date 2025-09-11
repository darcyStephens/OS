from mmu import MMU
import random


class RandMMU(MMU):
    def __init__(self, frames):
        super().__init__(frames)

    def replace(self, page_number):
        if page_number in self.tlb:
            self.log("Hit returning 1")
            return 1

        if len(self.tlb) < self.max_frames:
            self.tlb[page_number] = -1
            return -1

        if len(self.tlb) == self.max_frames:
            victim = random.choice(list(self.tlb.keys()))
            del self.tlb[victim]

            self.tlb[page_number] = -1

            if victim in self.dirty_pages:
                self.dirty_pages.remove(victim)
                return 0

            return -1
