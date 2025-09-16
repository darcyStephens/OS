from mmu import MMU
import time
import logging

class LruMMU(MMU):
    def __init__(self,frames):
        super().__init__(frames)


    def replace(self, page_number):
    
        if page_number in self.tlb:
            self.log(f"HIt updating for page {page_number}")
            self.tlb[page_number] = time.time_ns()
            return 1
        
        if len(self.tlb) < self.max_frames: 
            self.tlb[page_number] = time.time_ns()
            return -1
        
        if len(self.tlb) == self.max_frames:
            oldest_time = float('inf')
            oldest_key = -1
            for key, access_time in self.tlb.items():
                if access_time < oldest_time:
                    oldest_time = access_time
                    oldest_key = key
            
            if oldest_key != -1:
                del self.tlb[oldest_key]
                self.tlb[page_number] = time.time_ns()
            
            if oldest_key in self.dirty_pages:
                self.dirty_pages.remove(oldest_key)
                return 0
        
        return -1