'''
* Interface for Memory Management Unit.
* The memory management unit should maintain the concept of a page table.
* As pages are read and written to, this changes the pages loaded into the
* the limited number of frames. The MMU keeps records, which will be used
* to analyse the performance of different replacement strategies implemented
* for the MMU.
*
'''

import logging
class MMU:
    def __init__(self, frames):
        self.page_faults = 0
        self.disk_reads = 0
        self.disk_writes = 0
        self.debug = False
        self.max_frames = frames
        self.frames = {}  
        self.dirty_pages = set()  

    def read_memory(self, page_number):
        ret_val = self.replace(page_number)
        if ret_val == -1:  #Miss
            self.page_faults += 1
            self.disk_reads += 1
        elif ret_val == 0:  # Miss with dirty 
            self.page_faults += 1
            self.disk_reads += 1
            self.disk_writes += 1
        self.log(f"Read {page_number}: {'Hit' if ret_val == 1 else 'Miss'}")

    def write_memory(self, page_number):
        self.dirty_pages.add(page_number) 
        ret_val = self.replace(page_number)
        if ret_val == -1:
            self.page_faults += 1
            self.disk_reads += 1
        elif ret_val == 0:
            self.page_faults += 1
            self.disk_reads += 1
            self.disk_writes += 1
        self.log(f"Write {page_number}: {'Hit' if ret_val == 1 else 'Miss'}")
    def log(self,text:str):
        if not self.debug:
            return
        logging.debug(text)

    def replace(self,page_number):
        raise NotImplementedError("Method not implemented implement in subcalass")

    def set_debug(self):
        self.debug=True

    def reset_debug(self):
        self.debug=False

    def get_total_disk_reads(self):
        return self.disk_reads

    def get_total_disk_writes(self):
        return self.disk_writes

    def get_total_page_faults(self):
        return self.page_faults
