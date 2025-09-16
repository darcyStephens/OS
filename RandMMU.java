import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

/**
* MMU using random selection replacement strategy
*/

public class RandMMU implements MMU {

    public int PageFaults;
    public int DiskWrites;
    public int DiskReads;

    //stores the pages in the tlb
    public Set<Integer> TLB; 
    //tracks the dirty pages
    public Set<Integer> dirtyPages;


    public int MaxFrames;
    public boolean debug;

    public RandMMU(int frames) {
        PageFaults = 0;
        DiskReads = 0;
        DiskWrites = 0;
        TLB = new HashSet<>();
        dirtyPages = new HashSet<>();
        MaxFrames = frames;
        debug = false;
    }
    
    public void setDebug() {
        debug =true;
    }
    
    public void resetDebug() {
        debug =false;
    }
    
    
    public void readMemory(int page_number) {
        //todo
    }
    
    public void writeMemory(int page_number) {
        //todo
    }
    
    public int getTotalDiskReads() {
        //todo
        return -1;
    }
    
    public int getTotalDiskWrites() {
        //todo
        return -1;
    }
    
    public int getTotalPageFaults() {
        return -1;
    }
}