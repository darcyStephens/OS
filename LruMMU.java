import java.util.HashMap;
import java.sql.Time;
//import java.time.Instant;

/**
* MMU using least recently used replacement strategy
*/

public class LruMMU implements MMU {
    public int PageFaults;
    public int DiskWrites;
    public int DiskReads;


    //stores the PageNumber:time, each time a page is hit, update their time
    public HashMap<Integer, Long> TLB; 

    public int MaxFrames;

    public LruMMU(int frames) {
        PageFaults = 0;
        DiskReads = 0;
        DiskWrites = 0;
        TLB = new HashMap<>();
        MaxFrames = frames;
    }
    
    
    /*
    If page is in TLB, updates its access time and return -1
    if page not in TLB, adds page in, replacing as needed and returns -1
    increment PageFaults, DiskReads and DiskWrites based on the output of this function
    */ 
    public int CheckAndInsertPage(int page_number)
    {
        //page hit, update time since it has been used
        if(TLB.containsKey(page_number))
        {
            TLB.replace(page_number, System.nanoTime());
            return 1;
        }

        if(TLB.size() < MaxFrames)
        {
            TLB.put(page_number, System.nanoTime());
            return -1;
        }

        //cache full evict least recently used
        if (TLB.size() == MaxFrames)
        {
            //find oldest entry
            Long OldestEntry = Long.MAX_VALUE;
            int OldestKey = -1;
            for(Integer keys : TLB.keySet())
            {
                Long current = TLB.get(keys);
                if(current < OldestEntry)
                {
                    OldestEntry = current;
                    OldestKey = keys;
                }
            }
            
            if(OldestKey != -1)
            {
                //remove oldest entry
                TLB.remove(OldestKey);
                //add in new page
                TLB.put(page_number, System.nanoTime());
            }
            else
            {
                //error
            }
            
        }
        return -1;
    }

    public void setDebug() {
        //todo
    }
    
    public void resetDebug() {
        //todo
    }
    
    public void readMemory(int page_number) {
        //todo
    }
    
    public void writeMemory(int page_number) {
        //todo
    }
    
    public int getTotalDiskReads() {
        //todo
        return DiskReads;
    }
    
    public int getTotalDiskWrites() {
        //todo
        return DiskWrites;
    }
    
    public int getTotalPageFaults() {
        return PageFaults;
    }
}