from mmu import MMU
from collections import deque


class ClockMMU(MMU):
    def __init__(self, frames):
        super().__init__(frames)
        self.clock_hand = 0
        self.circle = deque(maxlen=frames)

    def replace(self, page_number):
        #return 1 if hit
        #return -1 if miss
        #return 0 if dirty miss

        #hit set the page used bit to true in the TLB
        if page_number in self.tlb:
            self.log("Hit, set use flag")
            self.tlb[page_number] = True
            return 1

        #forced miss, space available and page not in frame
        if len(self.tlb) < self.max_frames:
            self.log("Miss, space available adding to table")
            self.tlb[page_number] = True
            #add page to queue
            self.circle.append(page_number)
            self.log(f"  Loaded {page_number}")
            return -1

        start = self.clock_hand
        while True:
            #get the page at the front of the queue
            victim = self.circle[self.clock_hand]
            if not self.tlb[victim]:
                #page used bit is false, remove page
                return self._evict(victim, page_number)

            else:
                #page used bit is true
                self.log(f"Use flag true giving second chance for {victim}")
                #set to false
                self.tlb[victim] = False
                #increment the clock
                self.clock_hand = (self.clock_hand + 1) % self.max_frames

            self.log("Full circle with clock clear the start")
            #everything has been used just remove whatever is at the front
            if self.clock_hand == start:
                victim = self.circle[self.clock_hand]
                return self._evict(victim, page_number)

    def _evict(self, victim, page_number):
        was_dirty = victim in self.dirty_pages
        if was_dirty:
            self.dirty_pages.remove(victim)

        del self.tlb[victim]
        self.circle[self.clock_hand] = page_number
        self.tlb[page_number] = True
        self.clock_hand = (self.clock_hand + 1) % self.max_frames
        self.log(f"  Evicted {victim} (dirty: {was_dirty})")

        return 0 if was_dirty else -1
