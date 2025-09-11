from mmu import MMU
from collections import deque
class ClockMMU(MMU):
    def __init__(self,frames):
        super().__init__(frames)
        self.clock_hand=0
        self.circle=deque(maxlen=frames)


    def replace(self, page_number):


        if page_number in self.tlb:
            self.log("Hit, set use flag")
            self.tlb[page_number]['use'] = 1
            return 1

        if len(self.tlb)<self.max_frames:
            self.log("Miss, space available adding to table")
            self.tlb[page_number] = {'use': 1, 'dirty': 1 if page_number in self.dirty_pages else 0}
            self.circle.append(page_number)
            if page_number in self.dirty_pages:
                self.dirty_pages.remove(page_number)
            self.log(f"  Loaded {page_number}")
            return -1
        
        start=self.clock_hand
        while True:
            victim = self.circle[self.clock_hand]
            if self.tlb[victim]['use'] == 0:
                return self._evict(victim,page_number)

            else:
                self.log(f"Use flag 1 giving second chance for {victim}")
                self.tlb[victim]['use'] = 0
                self.clock_hand = (self.clock_hand + 1) % self.max_frames
            
            self.log("Full circle with clock clear the start")
            if self.clock_hand == start:
                victim = self.circle[self.clock_hand]
                return self._evict(victim,page_number)



    def _evict(self,victim,page_number):

        dirty = self.tlb[victim]['dirty']

        del self.tlb[victim]

        self.circle[self.clock_hand] = page_number

        self.tlb[page_number] = {'use': 1, 'dirty': 1 if page_number in self.dirty_pages else 0}
        self.clock_hand = (self.clock_hand + 1) % self.max_frames
        self.log(f"  Evicted {victim} (dirty: {dirty})")
        if page_number in self.dirty_pages:
            self.dirty_pages.remove(page_number)
        return 0 if dirty else -1





        