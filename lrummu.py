from mmu import MMU
import time
import logging

class LruMMU(MMU):
    def __init__(self,frames):
        super().__init__(frames)


    def replace(self, page_number):
        #return 1 if hit
        #return -1 if miss
        #return 0 if dirty miss

        #hit a page in the page table
        if page_number in self.tlb:
            self.log(f"HIt updating for page {page_number}")
            self.tlb[page_number] = time.time_ns()
            return 1
        
        #forced missed, tlb not full and page not already there
        if len(self.tlb) < self.max_frames: 
            #key is the page number, value is time of insertion
            self.tlb[page_number] = time.time_ns()
            return -1
        
        
        if len(self.tlb) == self.max_frames:
            oldest_time = float('inf')
            oldest_key = -1

            #find the least recently used (LRU) page in the TLB
            for key, access_time in self.tlb.items():
                if access_time < oldest_time:
                    oldest_time = access_time
                    oldest_key = key
            
            if oldest_key != -1:
                #remove LRU from tlb
                del self.tlb[oldest_key]
                #add in new page to replace old one
                self.tlb[page_number] = time.time_ns()
            
            if oldest_key in self.dirty_pages:
                #page to remove was dirty
                self.dirty_pages.remove(oldest_key)
                return 0
        
        return -1